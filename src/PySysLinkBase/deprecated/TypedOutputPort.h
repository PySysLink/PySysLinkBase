#ifndef PY_SYS_LINK_BASE_DEPRECATED_TYPED_OUTPUT_PORT
#define PY_SYS_LINK_BASE_DEPRECATED_TYPED_OUTPUT_PORT


#include "IUnknownTypeInputPort.h"

namespace PySysLinkBase
{
    template <typename T> 
    class TypedOutputPort : public IUnknownTypeOutputPort
    {
        private:
            T value;
        public:
            TypedOutputPort(T initialValue)
            {
                this->value = initialValue;
            }

            const std::string& GetTypeId() const
            {
                return << typeid(T).hash_code() << typeid(T).name();
            }
            
            const T GetValue() const
            {
                return this->value;
            }

            void SetValue(T newValue)
            {
                this->value = newValue;
            }
    };
} // namespace PySysLinkBase


#endif /* PY_SYS_LINK_BASE_DEPRECATED_TYPED_OUTPUT_PORT */
