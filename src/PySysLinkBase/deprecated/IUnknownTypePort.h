#ifndef PY_SYS_LINK_BASE_IUNKNOWN_TYPE_PORT
#define PY_SYS_LINK_BASE_IUNKNOWN_TYPE_PORT


#include <string>
#include "../SampleTime.h"

namespace PySysLinkBase
{
    class IUnknownTypePort {
    private:
        SampleTime sampleTime;
    public:
        IUnknownTypePort(SampleTime sampleTime);
        virtual const std::string& GetTypeId() const;
        virtual const SampleTime& GetSampleTime() const;
    };
}

#endif /* PY_SYS_LINK_BASE_IUNKNOWN_TYPE_PORT */
