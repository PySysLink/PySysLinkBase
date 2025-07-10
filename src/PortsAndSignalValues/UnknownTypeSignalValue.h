#ifndef PY_SYS_LINK_BASE_PORTS_AND_SIGNAL_VALUES_UNKNOWN_TYPE_SIGNAL_VALUE
#define PY_SYS_LINK_BASE_PORTS_AND_SIGNAL_VALUES_UNKNOWN_TYPE_SIGNAL_VALUE

#include <string>
#include <memory>
#include <stdexcept>

namespace PySysLinkBase
{
    template <typename T> 
    class SignalValue; // Forward declaration

    class UnknownTypeSignalValue
    {

        public:

            virtual ~UnknownTypeSignalValue() = default;
            virtual const std::string GetTypeId() const = 0;

            template <typename T>
            std::unique_ptr<SignalValue<T>> TryCastToTyped()
            {
                SignalValue<T>* typedPtr = dynamic_cast<SignalValue<T>*>(this);
                
                if (!typedPtr) throw std::bad_cast();

                return std::make_unique<SignalValue<T>>(*typedPtr);
            }

            virtual std::unique_ptr<UnknownTypeSignalValue> clone() const = 0;
    };
} // namespace PySysLinkBase

#endif /* PY_SYS_LINK_BASE_PORTS_AND_SIGNAL_VALUES_UNKNOWN_TYPE_SIGNAL_VALUE */
