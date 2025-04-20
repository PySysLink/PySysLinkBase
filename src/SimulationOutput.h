#ifndef SRC_SIMULATION_OUTPUT
#define SRC_SIMULATION_OUTPUT

#include <vector>
#include <memory>
#include <map>
#include <string>

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

    class SimulationOutput
    {
        public:
        std::map<std::string, std::map<std::string, std::shared_ptr<UnknownTypeSignal>>> signals;
    
        template<typename T>
        void InsertValueTyped(const std::string& signalType, const std::string& signalId, T value, double currentTime)
        {
            if (this->signals.find(signalType) == this->signals.end())
            {
                this->signals.insert({signalType, std::map<std::string, std::shared_ptr<UnknownTypeSignal>>()});
            }
            
            if (this->signals[signalType].find(signalId) == this->signals[signalType].end())
            {
                this->signals[signalType].insert({signalId, std::make_shared<Signal<T>>()});
            }
            this->signals[signalType][signalId]->TryInsertValue<T>(currentTime, value);
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
