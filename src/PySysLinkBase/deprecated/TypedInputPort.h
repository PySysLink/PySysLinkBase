#ifndef PY_SYS_LINK_BASE_TYPED_INPUT_PORT
#define PY_SYS_LINK_BASE_TYPED_INPUT_PORT

#include "IUnknownTypeInputPort.h"
#include <typeinfo>

namespace PySysLinkBase
{
    template <typename T> 
    class TypedInputPort : public IUnknownTypeInputPort
    {
        private:
            T value;
        public:
            TypedInputPort(T initialValue)
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



#endif /* PY_SYS_LINK_BASE_TYPED_INPUT_PORT */
