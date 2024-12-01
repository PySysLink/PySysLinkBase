#ifndef SRC_PY_SYS_LINK_BASE_PORTS_AND_SIGNAL_VALUES_SIGNAL_VALUE
#define SRC_PY_SYS_LINK_BASE_PORTS_AND_SIGNAL_VALUES_SIGNAL_VALUE

#include <string>
#include "UnknownTypeSignalValue.h"
#include <memory>

namespace PySysLinkBase
{
    template <typename T> 
    class SignalValue : public UnknownTypeSignalValue
    {
        private:
            T payload;
        public:
            SignalValue(T initialPayload) : payload(initialPayload) {}

            SignalValue(const SignalValue& other) = default;

            std::unique_ptr<UnknownTypeSignalValue> clone() const override {
                return std::make_unique<SignalValue<T>>(*this);
            }

            const std::string GetTypeId() const
            {
                return std::to_string(typeid(T).hash_code()) + typeid(T).name();
            }

            const T GetPayload() const
            {
                return this->payload;
            }

            void SetPayload(T newPayload)
            {
                this->payload = newPayload;
            }
    };
} // namespace PySysLinkBase


#endif /* SRC_PY_SYS_LINK_BASE_PORTS_AND_SIGNAL_VALUES_SIGNAL_VALUE */
