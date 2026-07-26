#include "SimulationOutput.h"
#include <highfive/H5Easy.hpp>
#include <highfive/eigen.hpp>
#include <highfive/H5File.hpp>
#include <fstream>
#include <variant>
#include <complex>

namespace PySysLinkBase
{
    SimulationOutput::SimulationOutput(bool saveToVectors, bool saveToFileContinuously, std::string hdf5FileName)
        : saveToVectors(saveToVectors),
        saveToFileContinuously(saveToFileContinuously),
        hdf5FileName(std::move(hdf5FileName)),
        hdf5File(nullptr),         
        dumpOptions(nullptr)
    {
        if (this->saveToFileContinuously)
        {
            // Use actual types here
            this->dumpOptions = new H5Easy::DumpOptions();
            static_cast<H5Easy::DumpOptions*>(this->dumpOptions)->setChunkSize({1024});
            this->hdf5File = std::make_shared<H5Easy::File>(this->hdf5FileName, H5Easy::File::Overwrite);
            this->writeTasks.clear();

            this->ioThread = std::thread([this]{
                WriteTask task;
                while (this->taskQueue.pop(task))
                {
                    std::visit([&](auto&& arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, bool>)
                        {
                            for (const auto& value : task.values)
                            {
                                H5Easy::dump(*static_cast<H5Easy::File*>(this->hdf5File.get()),
                                    task.datasetPath + "/values",
                                    static_cast<int>(std::get<T>(*value)),
                                    {lastFlushedIndex[task.datasetPath]},
                                    *static_cast<H5Easy::DumpOptions*>(this->dumpOptions));
                                lastFlushedIndex[task.datasetPath] += 1;
                            }
                        }
                        else if constexpr (std::is_same_v<T, IntMatrix> || std::is_same_v<T, DoubleMatrix> || std::is_same_v<T, BoolMatrix> || std::is_same_v<T, ComplexMatrix>) {
                            auto& file = *static_cast<H5Easy::File*>(this->hdf5File.get());

                            const std::string path = task.datasetPath + "/values";

                            for (const auto& value : task.values)
                            {
                                const auto& mat = std::get<T>(*value);

                                //----------------------------------------------------------
                                // First sample: create dataset
                                //----------------------------------------------------------

                                if (matrixDatasets.find(path) == matrixDatasets.end())
                                {
                                    matrixSizes[path] = {
                                        static_cast<size_t>(mat.rows()),
                                        static_cast<size_t>(mat.cols())
                                    };

                                    HighFive::DataSpace dataspace(
                                        {
                                            0,
                                            static_cast<size_t>(mat.rows()),
                                            static_cast<size_t>(mat.cols())
                                        },
                                        {
                                            HighFive::DataSpace::UNLIMITED,
                                            static_cast<size_t>(mat.rows()),
                                            static_cast<size_t>(mat.cols())
                                        }
                                    );

                                    HighFive::DataSetCreateProps props;

                                    props.add(HighFive::Chunking({
                                        1,
                                        static_cast<size_t>(mat.rows()),
                                        static_cast<size_t>(mat.cols())
                                    }));

                                    matrixDatasets[path] =
                                        file.createDataSet<typename T::Scalar>(
                                            path,
                                            dataspace,
                                            props);
                                }

                                //----------------------------------------------------------
                                // Check dimensions
                                //----------------------------------------------------------

                                auto [rows, cols] = matrixSizes[path];

                                if (rows != static_cast<size_t>(mat.rows()) ||
                                    cols != static_cast<size_t>(mat.cols()))
                                {
                                    throw std::runtime_error(
                                        "Matrix dimensions changed for signal '" + path + "'");
                                }

                                //----------------------------------------------------------
                                // Append sample
                                //----------------------------------------------------------

                                auto& dataset = matrixDatasets[path];

                                std::size_t sample = lastFlushedIndex[path];

                                dataset.resize({
                                    sample + 1,
                                    rows,
                                    cols
                                });

                                dataset.select(
                                    {sample,0,0},
                                    {1,rows,cols}
                                ).write(mat);

                                lastFlushedIndex[path]++;
                            }
                        }
                        else
                        {
                            for (const auto& value : task.values)
                            {
                                H5Easy::dump(*static_cast<H5Easy::File*>(this->hdf5File.get()),
                                    task.datasetPath + "/values",
                                    std::get<T>(*value),
                                    {lastFlushedIndex[task.datasetPath]},
                                    *static_cast<H5Easy::DumpOptions*>(this->dumpOptions));
                                lastFlushedIndex[task.datasetPath] += 1;
                            }
                        }
                    }, *task.values[0]);

                    lastFlushedIndex[task.datasetPath] -= task.times.size();

                    for (const auto& time : task.times)
                    {
                        H5Easy::dump(*static_cast<H5Easy::File*>(this->hdf5File.get()),
                            task.datasetPath + "/times",
                            time,
                            {lastFlushedIndex[task.datasetPath]},
                            *static_cast<H5Easy::DumpOptions*>(this->dumpOptions));
                        lastFlushedIndex[task.datasetPath] += 1;
                    }
                }
                static_cast<H5Easy::File*>(this->hdf5File.get())->flush();
            });
        }
        this->lastFlushedIndex.clear();
    }

    SimulationOutput::~SimulationOutput() {
        if (saveToFileContinuously) {
            for (auto& [path, task] : writeTasks) {
                if (task.currentIndex > 0) {
                    WriteTask finalTask;
                    finalTask.datasetPath = task.datasetPath;
                    finalTask.times.assign(
                        task.times.begin(),
                        task.times.begin() + task.currentIndex
                    );
                    finalTask.values.assign(
                        task.values.begin(),
                        task.values.begin() + task.currentIndex
                    );
                    finalTask.currentIndex = finalTask.times.size();
                    taskQueue.push(finalTask);
                }
            }
            taskQueue.shutdown();
            if (ioThread.joinable()) {
                ioThread.join();
            }
        }
        signals.clear();
        
        // Fix 2: Properly delete dumpOptions
        if (dumpOptions) {
            delete static_cast<H5Easy::DumpOptions*>(dumpOptions);
            dumpOptions = nullptr;
        }
    }

    void SimulationOutput::InsertUnknownValue(
        const std::string& signalType,
        const std::string& signalId,
        const std::shared_ptr<PySysLinkBase::UnknownTypeSignalValue>& value,
        double currentTime)
    {
        FullySupportedSignalValue fullySupportedValue = ConvertToFullySupportedSignalValue(value);
        this->InsertFullySupportedValue(signalType, signalId, fullySupportedValue, currentTime);
    }

    void SimulationOutput::InsertFullySupportedValue(
        const std::string& signalType,
        const std::string& signalId,
        const FullySupportedSignalValue& value,
        double currentTime)
    {
        std::visit(
            [&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                this->InsertValueTyped<T>(signalType, signalId, arg, currentTime);
            },
            value);
    }

    void SimulationOutput::WriteJson(const std::string& filename) const
    {
        std::ofstream out(filename);

        out << "{";

        bool firstType = true;

        for (const auto& [signalType, innerMap] : signals)
        {
            if (!firstType)
                out << ",";

            firstType = false;

            out << "\n  \"" << escapeJson(signalType) << "\": {";

            bool firstSignal = true;

            for (const auto& [signalId, signal] : innerMap)
            {
                if (!firstSignal)
                    out << ",";

                firstSignal = false;

                out << "\n    \"" << escapeJson(signalId) << "\": {";

                //------------------------------------------------------
                // Times
                //------------------------------------------------------

                out << "\n      \"times\": [";

                for (size_t i = 0; i < signal->times.size(); ++i)
                {
                    if (i)
                        out << ",";

                    out << signal->times[i];
                }

                out << "],";

                //------------------------------------------------------
                // Values
                //------------------------------------------------------

                out << "\n      \"values\": ";

                signal->WriteValuesJson(out);

                out << "\n    }";
            }

            out << "\n  }";
        }

        out << "\n}\n";
    }
}