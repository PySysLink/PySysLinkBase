#ifndef SRC_PY_SYS_LINK_BASE_PORTS_AND_SIGNAL_VALUES_SIGNAL_VALUE
#define SRC_PY_SYS_LINK_BASE_PORTS_AND_SIGNAL_VALUES_SIGNAL_VALUE

#include <string>
#include "UnknownTypeSignalValue.h"
#include <memory>
#include <optional>

namespace PySysLinkBase
{
    template <typename T> 
    class SignalValue : public UnknownTypeSignalValue
    {
        private:
            std::optional<T> payload;
        public:
            SignalValue() = default;
            
            SignalValue(T initialPayload) : payload(initialPayload) {}

            SignalValue(const SignalValue& other) = default;

            std::unique_ptr<UnknownTypeSignalValue> clone() const override {
                return std::make_unique<SignalValue<T>>(*this);
            }

            const std::string GetTypeId() const
            {
                return std::to_string(typeid(T).hash_code()) + typeid(T).name();
            }

            bool IsInitialized() const {
                return payload.has_value();
            }

            const T GetPayload() const
            {
                if (!payload) {
                    throw std::runtime_error("SignalValue accessed before initialization");
                }
                return *payload;
            }

            void SetPayload(T newPayload)
            {
                this->payload = newPayload;
            }
    };
} // namespace PySysLinkBase


#endif /* SRC_PY_SYS_LINK_BASE_PORTS_AND_SIGNAL_VALUES_SIGNAL_VALUE */
