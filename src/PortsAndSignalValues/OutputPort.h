#ifndef PY_SYS_LINK_BASE_PORTS_AND_SIGNAL_VALUES_OUTPUT_PORT
#define PY_SYS_LINK_BASE_PORTS_AND_SIGNAL_VALUES_OUTPUT_PORT


#include "Port.h"

namespace PySysLinkBase
{
    class OutputPort : public Port {
        public:
            OutputPort(std::unique_ptr<UnknownTypeSignalValue> value, ISimulationBlock& ownerBlock);
    };
}

#endif /* PY_SYS_LINK_BASE_PORTS_AND_SIGNAL_VALUES_OUTPUT_PORT */
