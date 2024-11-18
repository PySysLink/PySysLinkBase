#ifndef PY_SYS_LINK_BASE_PORT_LINK
#define PY_SYS_LINK_BASE_PORT_LINK

#include "PortsAndSignalValues/InputPort.h"
#include "PortsAndSignalValues/OutputPort.h"

namespace PySysLinkBase
{
    class PortLink
    {
    public:
        PortLink(const OutputPort& origin, const InputPort& sink) : origin(origin), sink(sink) {}

        const OutputPort& origin;
        const InputPort& sink;
    };
}

#endif /* PY_SYS_LINK_BASE_PORT_LINK */
