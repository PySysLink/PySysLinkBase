#ifndef PY_SYS_LINK_BASE_PORTS_AND_SIGNAL_VALUES_INPUT_PORT
#define PY_SYS_LINK_BASE_PORTS_AND_SIGNAL_VALUES_INPUT_PORT

#include "Port.h"

namespace PySysLinkBase
{
    class InputPort : public Port {
    private:
        bool hasDirectFeedthrough;
    public:
        InputPort(bool hasDirectFeedthrough, std::unique_ptr<UnknownTypeSignalValue> value, ISimulationBlock& ownerBlock);
        const bool HasDirectFeedtrough() const;    

        bool IsInputDirectBlockChainEnd() const;    
    };
}

#endif /* PY_SYS_LINK_BASE_PORTS_AND_SIGNAL_VALUES_INPUT_PORT */
