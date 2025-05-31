#include "SimulationOutput.h"
#include <highfive/H5Easy.hpp>
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
        if (dumpOptions) delete static_cast<H5Easy::DumpOptions*>(dumpOptions);
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

    void SimulationOutput::WriteJson(const std::string& filename) const {
        std::ofstream out(filename);
        out << "{";

        bool firstType = true;
        for (const auto& [signalType, innerMap] : signals) {
            if (!firstType) out << ",";
            firstType = false;

            out << "\n  \"" << escapeJson(signalType) << "\": {";

            bool firstSig = true;
            for (const auto& [signalId, unkPtr] : innerMap) {
                if (!firstSig) out << ",";
                firstSig = false;

                out << "\n    \"" << escapeJson(signalId) << "\": {";

                out << "\n      \"times\": [";
                const auto& times = unkPtr->times;
                for (size_t i = 0; i < times.size(); ++i) {
                    if (i) out << ", ";
                    out << times[i];
                }
                out << "],";

                out << "\n      \"values\": [";
                bool firstV = true;
                if (auto p = dynamic_cast<Signal<double>*>(unkPtr.get())) {
                    for (double v : p->values) {
                        if (!firstV) out << ", ";
                        firstV = false;
                        out << v;
                    }
                }
                else if (auto p = dynamic_cast<Signal<int>*>(unkPtr.get())) {
                    for (int v : p->values) {
                        if (!firstV) out << ", ";
                        firstV = false;
                        out << v;
                    }
                }
                else if (auto p = dynamic_cast<Signal<bool>*>(unkPtr.get())) {
                    for (bool v : p->values) {
                        if (!firstV) out << ", ";
                        firstV = false;
                        out << (v ? "true" : "false");
                    }
                }
                else if (auto p = dynamic_cast<Signal<std::string>*>(unkPtr.get())) {
                    for (const auto& v : p->values) {
                        if (!firstV) out << ", ";
                        firstV = false;
                        out << "\"" << escapeJson(v) << "\"";
                    }
                }
                else if (auto p = dynamic_cast<Signal<std::complex<double>>*>(unkPtr.get())) {
                    for (const auto& cval : p->values) {
                        if (!firstV) out << ", ";
                        firstV = false;
                        out << "{"
                            << "\"real\":" << cval.real() << ", "
                            << "\"imag\":" << cval.imag()
                            << "}";
                    }
                }
                out << "]";

                out << "\n    }";
            }

            out << "\n  }";
        }

        out << "\n}\n";
    }
}