#ifndef SRC_BLOCK_EVENTS_DISPLAY_UPDATE_BLOCK_EVENT
#define SRC_BLOCK_EVENTS_DISPLAY_UPDATE_BLOCK_EVENT

#include <string>
#include "ConfigurationValue.h"
#include "BlockEvent.h"

namespace PySysLinkBase
{
    class DisplayUpdateBlockEvent : public BlockEvent
    {
        public:
        double simulationTime;
        std::string displayId;
        ConfigurationValue value;
        DisplayUpdateBlockEvent(double simulationTime, std::string displayId, ConfigurationValue value) : simulationTime(simulationTime), displayId(displayId), value(value), BlockEvent("DisplayUpdate") {}

        ~DisplayUpdateBlockEvent() = default;
    };
} // namespace PySysLinkBase
#endif /* SRC_BLOCK_EVENTS_DISPLAY_UPDATE_BLOCK_EVENT */
