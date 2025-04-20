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

    struct SimulationOutput
    {
        std::map<std::string, std::map<std::string, std::shared_ptr<UnknownTypeSignal>>> signals;

        void AddValue(const std::string signalType, const std::string signalId, const std::shared_ptr<UnknownTypeSignalValue> value, double currentTime)
        {
            if (this->signals.find(signalType) == this->signals.end())
            {
                this->signals.insert({signalType, std::map<std::string, std::shared_ptr<UnknownTypeSignal>>()});
            }
            
            try
            {
                double valueDouble = value->TryCastToTyped<double>()->GetPayload();

                if (this->signals[signalType].find(signalId) == this->signals[signalType].end())
                {
                    this->signals[signalType].insert({signalId, std::make_shared<Signal<double>>()});
                }
                this->signals[signalType][signalId]->TryInsertValue<double>(currentTime, valueDouble);
            }
            catch(const std::bad_variant_access& e)
            {
                std::complex<double> valueComplex = value->TryCastToTyped<std::complex<double>>()->GetPayload();

                if (this->signals[signalType].find(signalId) == this->signals[signalType].end())
                {
                    this->signals[signalType].insert({signalId, std::make_shared<Signal<std::complex<double>>>()});
                }
                this->signals[signalType][signalId]->TryInsertValue<std::complex<double>>(currentTime, valueComplex);
            }
        }
        
    };
} // namespace PySysLinkBase


#endif /* SRC_SIMULATION_OUTPUT */
