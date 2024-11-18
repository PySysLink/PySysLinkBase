#ifndef PY_SYS_LINK_BASE_DEPRECATED_IUNKNOWN_TYPE_INPUT_PORT
#define PY_SYS_LINK_BASE_DEPRECATED_IUNKNOWN_TYPE_INPUT_PORT


#include <string>
#include "IUnknownTypePort.h"
#include "TypedInputPort.h"

namespace PySysLinkBase
{
    class IUnknownTypeInputPort : public IUnknownTypePort {
    public:
        virtual const std::string& GetTypeId() const;

        virtual bool HasDirectFeedthrough();

        template <typename T>
        TypedInputPort<T> TryCastToTyped()
        {
            TypedInputPort<T> typedPort = dynamic_cast<TypedInputPort<T>>(this);
            if (!typedPort) throw std::bad_cast();
            return typedPort;
        }
    };
}


#endif /* PY_SYS_LINK_BASE_DEPRECATED_IUNKNOWN_TYPE_INPUT_PORT */
