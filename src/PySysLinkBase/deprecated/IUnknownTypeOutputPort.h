#ifndef PY_SYS_LINK_BASE_IUNKNOWN_TYPE_OUTPUT_PORT
#define PY_SYS_LINK_BASE_IUNKNOWN_TYPE_OUTPUT_PORT

#include <string>
#include "IUnknownTypePort.h"
#include "TypedOutputPort.h"

namespace PySysLinkBase
{
    class IUnknownTypeOutputPort : public IUnknownTypePort {
    public:
        virtual const std::string& GetTypeId() const;

        template <typename T>
        TypedOutputPort<T> TryCastToTyped()
        {
            TypedOutputPort<T> typedPort = dynamic_cast<TypedOutputPort<T>>(this);
            if (!typedPort) throw std::bad_cast();
            return typedPort;
        }
    };
}



#endif /* PY_SYS_LINK_BASE_IUNKNOWN_TYPE_OUTPUT_PORT */
