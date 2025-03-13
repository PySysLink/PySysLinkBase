#ifndef SRC_BLOCK_EVENTS_VALUE_UPDATE_BLOCK_EVENT
#define SRC_BLOCK_EVENTS_VALUE_UPDATE_BLOCK_EVENT

#include <string>
#include "../FullySupportedSignalValue.h"
#include "BlockEvent.h"

namespace PySysLinkBase
{
    class ValueUpdateBlockEvent : public BlockEvent
    {
        public:
        double simulationTime;
        std::string valueId;
        FullySupportedSignalValue value;
        ValueUpdateBlockEvent(double simulationTime, std::string valueId, FullySupportedSignalValue value) : simulationTime(simulationTime), valueId(valueId), value(value), BlockEvent("ValueUpdate") {}

        ~ValueUpdateBlockEvent() = default;
    };
} // namespace PySysLinkBase
#endif /* SRC_BLOCK_EVENTS_VALUE_UPDATE_BLOCK_EVENT */
