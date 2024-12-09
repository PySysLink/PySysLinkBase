#ifndef SRC_PORTS_AND_SIGNAL_VALUES_INPUT_PORT
#define SRC_PORTS_AND_SIGNAL_VALUES_INPUT_PORT

#include "Port.h"

namespace PySysLinkBase
{
    class InputPort : public Port {
    private:
        bool hasDirectFeedthrough;
    public:
        InputPort(bool hasDirectFeedthrough, std::unique_ptr<UnknownTypeSignalValue> value);
        const bool HasDirectFeedtrough() const;    
    };
}

#endif /* SRC_PORTS_AND_SIGNAL_VALUES_INPUT_PORT */
