#ifndef SRC_PORTS_AND_SIGNAL_VALUES_OUTPUT_PORT
#define SRC_PORTS_AND_SIGNAL_VALUES_OUTPUT_PORT


#include "Port.h"

namespace PySysLinkBase
{
    class OutputPort : public Port {
        public:
            OutputPort(std::shared_ptr<UnknownTypeSignalValue> value);
    };
}

#endif /* SRC_PORTS_AND_SIGNAL_VALUES_OUTPUT_PORT */
