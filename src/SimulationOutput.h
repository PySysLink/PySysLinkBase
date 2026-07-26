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
#include <iomanip>  
#include <sstream>
#include <fstream>
#include <typeinfo>  
#include <typeindex> 
#include <highfive/H5DataSet.hpp>

#include "PortsAndSignalValues/UnknownTypeSignalValue.h"
#include "FullySupportedSignalValue.h"

namespace PySysLinkBase
{
    static std::string escapeJson(const std::string& s) {
        std::ostringstream o;
        for (char c : s) {
            switch (c) {
                case '\"': o << "\\\""; break;
                case '\\': o << "\\\\"; break;
                case '\b': o << "\\b";  break;
                case '\f': o << "\\f";  break;
                case '\n': o << "\\n";  break;
                case '\r': o << "\\r";  break;
                case '\t': o << "\\t";  break;
                default:
                    if (static_cast<unsigned char>(c) < 0x20) {
                        // control characters → \u00XX
                        o << "\\u"
                          << std::hex << std::setw(4) << std::setfill('0')
                          << (int)(unsigned char)c;
                    } else {
                        o << c;
                    }
            }
        }
        return o.str();
    }

    template<typename T>
    inline void WriteJsonValue(std::ostream& out, const T& value)
    {
        out << value;
    }

    template<>
    inline void WriteJsonValue<bool>(std::ostream& out, const bool& value)
    {
        out << (value ? "true" : "false");
    }

    template<>
    inline void WriteJsonValue<std::string>(std::ostream& out,
                                        const std::string& value)
    {
        out << "\"" << escapeJson(value) << "\"";
    }

    template<>
    inline void WriteJsonValue<std::complex<double>>(std::ostream& out,
                                                    const std::complex<double>& value)
    {
        out << "{";
        out << "\"real\":" << value.real() << ",";
        out << "\"imag\":" << value.imag();
        out << "}";
    }

    template<typename Derived>
    inline void WriteMatrixJson(std::ostream& out,
                                const Eigen::MatrixBase<Derived>& matrix)
    {
        out << "[";

        for (Eigen::Index r = 0; r < matrix.rows(); ++r)
        {
            if (r)
                out << ",";

            out << "[";

            for (Eigen::Index c = 0; c < matrix.cols(); ++c)
            {
                if (c)
                    out << ",";

                WriteJsonValue(out, matrix(r, c));
            }

            out << "]";
        }

        out << "]";
    }

    template<>
    inline void WriteJsonValue<IntMatrix>(std::ostream& out,
                                        const IntMatrix& matrix)
    {
        WriteMatrixJson(out, matrix);
    }

    template<>
    inline void WriteJsonValue<DoubleMatrix>(std::ostream& out,
                                            const DoubleMatrix& matrix)
    {
        WriteMatrixJson(out, matrix);
    }

    template<>
    inline void WriteJsonValue<BoolMatrix>(std::ostream& out,
                                        const BoolMatrix& matrix)
    {
        WriteMatrixJson(out, matrix);
    }

    template<>
    inline void WriteJsonValue<ComplexMatrix>(std::ostream& out,
                                            const ComplexMatrix& matrix)
    {
        WriteMatrixJson(out, matrix);
    }

    template <typename T> 
    class Signal; // Forward declaration

    template <typename T> 
    class Signal; // Forward declaration

    class UnknownTypeSignal
    {
    public:
        virtual ~UnknownTypeSignal() = default;
        std::string id;
        std::vector<double> times;
        
        // Reserve capacity upfront
        UnknownTypeSignal() {
            times.reserve(4096); // Pre-allocate memory
        }

        virtual const std::string GetTypeId() const = 0;
        virtual void WriteValuesJson(std::ostream& out) const = 0;

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
            typedPtr->values.push_back(std::move(value)); // Use move semantics
        }
    };

    template <typename T> 
    class Signal : public UnknownTypeSignal
    {
    public:
        std::vector<T> values;
        
        Signal() {
            values.reserve(4096); // Pre-allocate memory
        }

        // Cache type ID to avoid repeated allocations
        const std::string GetTypeId() const override {
            const std::string typeId = 
                std::to_string(typeid(T).hash_code()) + typeid(T).name();
            return typeId;
        }

        void WriteValuesJson(std::ostream& out) const override
        {
            out << "[";

            bool first = true;

            for (const auto& value : values)
            {
                if (!first)
                    out << ",";

                first = false;

                WriteJsonValue(out, value);
            }

            out << "]";
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
        std::shared_ptr<void> hdf5File; // opaque pointer, actual type in .cpp
        void* dumpOptions; // opaque pointer, actual type in .cpp

        std::unordered_map<std::string, std::size_t> lastFlushedIndex;
        std::unordered_map<std::string, WriteTask> writeTasks;

        std::unordered_map<std::string, HighFive::DataSet> matrixDatasets;
        std::unordered_map<std::string, std::pair<size_t,size_t>> matrixSizes;
    public:
        SimulationOutput(bool saveToVectors=true, bool saveToFileContinuously=false, std::string hdf5FileName="");
        ~SimulationOutput();

        std::map<std::string, std::map<std::string, std::shared_ptr<UnknownTypeSignal>>> signals;

        template<typename T>
        void InsertValueTyped(const std::string& signalType, const std::string& signalId, T value, double currentTime);

        void InsertUnknownValue(
            const std::string& signalType,
            const std::string& signalId,
            const std::shared_ptr<PySysLinkBase::UnknownTypeSignalValue>& value,
            double currentTime);

        void InsertFullySupportedValue(
            const std::string& signalType,
            const std::string& signalId,
            const FullySupportedSignalValue& value,
            double currentTime);

        void WriteJson(const std::string& filename) const;
    }; 
    
    template<typename T>
    void SimulationOutput::InsertValueTyped(const std::string& signalType, const std::string& signalId, T value, double currentTime)
    {
        auto& signalMap = signals[signalType];
        auto& signalPtr = signalMap[signalId];
        
        if (!signalPtr) {
            // Use make_shared directly with concrete type
            signalPtr = std::make_shared<Signal<T>>();
            signalPtr->id = signalId;
        }
        
        // Add the value
        signalPtr->times.push_back(currentTime);
        static_cast<Signal<T>*>(signalPtr.get())->values.push_back(std::move(value));
    }
} // namespace PySysLinkBase


#endif /* SRC_SIMULATION_OUTPUT */
