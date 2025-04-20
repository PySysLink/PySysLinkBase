#ifndef SRC_SIMULATION_OUTPUT
#define SRC_SIMULATION_OUTPUT

#include <vector>
#include <memory>
#include <map>
#include <string>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>

#include <highfive/H5Easy.hpp>

#include "PortsAndSignalValues/UnknownTypeSignalValue.h"
#include "FullySupportedSignalValue.h"

namespace PySysLinkBase
{
    template <typename T> 
    class Signal; // Forward declaration

    class UnknownTypeSignal
    {
        public:
        std::string id;
        std::vector<double> times;
        
        virtual const std::string GetTypeId() const = 0;

        template <typename T>
        std::unique_ptr<Signal<T>> TryCastToTyped()
        {
            Signal<T>* typedPtr = dynamic_cast<Signal<T>*>(this);
            
            if (!typedPtr) throw std::bad_cast();

            return std::make_unique<Signal<T>>(*typedPtr);
        }

        template <typename T>
        void TryInsertValue(double time, T value)
        {
            Signal<T>* typedPtr = dynamic_cast<Signal<T>*>(this);
            
            if (!typedPtr) throw std::bad_cast();

            typedPtr->times.push_back(time);
            typedPtr->values.push_back(value);
        }
    };

    template <typename T> 
    class Signal : public UnknownTypeSignal
    {
        public:
        std::vector<T> values;

        const std::string GetTypeId() const
        {
            return std::to_string(typeid(T).hash_code()) + typeid(T).name();
        }
    };

    struct WriteTask {
        std::string datasetPath;
        int currentIndex = 0;
        std::vector<double> times;
        std::vector<std::shared_ptr<FullySupportedSignalValue>> values;

        WriteTask() : times(1024, 0.0), values(1024, nullptr) {}
    };

    template<typename Task>
    class TaskQueue {
    std::queue<Task> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool done_ = false;
    public:
    void push(Task const& t) {
        {
        std::lock_guard<std::mutex> lk(mutex_);
        queue_.push(t);
        }
        cv_.notify_one();
    }
    // Consumer pop; returns false if shutting down and queue empty
    bool pop(Task& out) {
        std::unique_lock<std::mutex> lk(mutex_);
        cv_.wait(lk, [&]{ return done_ || !queue_.empty(); });
        if (queue_.empty()) return false;
        out = std::move(queue_.front());
        queue_.pop();
        return true;
    }
    void shutdown() {
        {
        std::lock_guard<std::mutex> lk(mutex_);
        done_ = true;
        }
        cv_.notify_all();
    }
    };

    class SimulationOutput
    {
        private:

        std::thread ioThread;
        TaskQueue<WriteTask> taskQueue;

        bool saveToVectors;
        bool saveToFileContinuously;
        std::string hdf5FileName;
        std::shared_ptr<H5Easy::File> hdf5File;
        H5Easy::DumpOptions dumpOptions;

        std::unordered_map<std::string, std::size_t> lastFlushedIndex;
        std::unordered_map<std::string, WriteTask> writeTasks;


        public:
        SimulationOutput(bool saveToVectors=true, bool saveToFileContinuously=false, std::string hdf5FileName="")
        {
            this->saveToVectors = saveToVectors;
            this->saveToFileContinuously = saveToFileContinuously;
            this->hdf5FileName = hdf5FileName;
            if (this->saveToFileContinuously)
            {
                this->dumpOptions.setChunkSize({1024});
                this->hdf5File = std::make_shared<HighFive::File>(this->hdf5FileName, H5Easy::File::Overwrite);
                this->writeTasks = {};

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
                                    H5Easy::dump(*this->hdf5File,
                                        task.datasetPath + "/values",
                                        static_cast<int>(std::get<T>(*value)),
                                        {lastFlushedIndex[task.datasetPath]},
                                        this->dumpOptions);
                                    lastFlushedIndex[task.datasetPath] += 1;
                                }
                            }
                            else
                            {
                                for (const auto& value : task.values)
                                {
                                    H5Easy::dump(*this->hdf5File,
                                        task.datasetPath + "/values",
                                        std::get<T>(*value),
                                        {lastFlushedIndex[task.datasetPath]},
                                        this->dumpOptions);
                                    lastFlushedIndex[task.datasetPath] += 1;
                                }
                            }

                        }, *task.values[0]);
                        
                        lastFlushedIndex[task.datasetPath] -= task.times.size();

                        for (const auto& time : task.times)
                        {
                            H5Easy::dump(*this->hdf5File,
                                task.datasetPath + "/times",
                                time,
                                {lastFlushedIndex[task.datasetPath]},
                                this->dumpOptions);
                            lastFlushedIndex[task.datasetPath] += 1;
                        }
                    }
                    // Ensure any final data is flushed
                    this->hdf5File->flush();
                });
            }
            this->lastFlushedIndex.clear();
        }

        ~SimulationOutput() {
            if (saveToFileContinuously) {
                // 1) Push any remaining partial batches
                for (auto& [path, task] : writeTasks) {
                    if (task.currentIndex > 0) {
                        WriteTask finalTask;
                        finalTask.datasetPath = task.datasetPath;
                        // Copy only the valid prefix [0..currentIndex)
                        finalTask.times.assign(
                            task.times.begin(),
                            task.times.begin() + task.currentIndex
                        );
                        finalTask.values.assign(
                            task.values.begin(),
                            task.values.begin() + task.currentIndex
                        );
                        // Mark its actual size
                        finalTask.currentIndex = finalTask.times.size();
                        taskQueue.push(finalTask);
                    }
                }
                // 2) Signal the I/O thread to finish
                taskQueue.shutdown();
                if (ioThread.joinable()) {
                    ioThread.join();
                }
            }
        }

        std::map<std::string, std::map<std::string, std::shared_ptr<UnknownTypeSignal>>> signals;
    
        template<typename T>
        void InsertValueTyped(const std::string& signalType, const std::string& signalId, T value, double currentTime)
        {
            if (!this->saveToVectors)
            {   
                if (!signals.count(signalType))
                {
                    signals[signalType] = {};
                }
                if (!signals[signalType].count(signalId))
                {
                    signals[signalType][signalId] = std::make_shared<Signal<T>>();
                }
                
                this->signals[signalType][signalId]->TryInsertValue<T>(currentTime, value);
            }

            if (this->saveToFileContinuously)
            {
                if (!this->writeTasks.count(signalType + "/" + signalId) || this->writeTasks[signalType + "/" + signalId].currentIndex >= 1024)
                {
                    this->writeTasks[signalType + "/" + signalId] = WriteTask();
                    this->writeTasks[signalType + "/" + signalId].datasetPath = signalType + "/" + signalId;
                }

                this->writeTasks[signalType + "/" + signalId].times[this->writeTasks[signalType + "/" + signalId].currentIndex] = currentTime;
                this->writeTasks[signalType + "/" + signalId].values[this->writeTasks[signalType + "/" + signalId].currentIndex] = std::make_shared<FullySupportedSignalValue>(value);
                this->writeTasks[signalType + "/" + signalId].currentIndex++;

                if (this->writeTasks[signalType + "/" + signalId].currentIndex >= 1024)
                {
                    this->taskQueue.push(this->writeTasks[signalType + "/" + signalId]);
                    this->writeTasks[signalType + "/" + signalId].currentIndex = 0;
                }
            }
        }  
        
        void InsertUnknownValue(
            const std::string& signalType,
            const std::string& signalId,
            const std::shared_ptr<PySysLinkBase::UnknownTypeSignalValue>& value,
            double currentTime)
        {
            FullySupportedSignalValue fullySupportedValue = ConvertToFullySupportedSignalValue(value);
            this->InsertFullySupportedValue(signalType, signalId, fullySupportedValue, currentTime);
        }

        void InsertFullySupportedValue(
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
    };
} // namespace PySysLinkBase


#endif /* SRC_SIMULATION_OUTPUT */
