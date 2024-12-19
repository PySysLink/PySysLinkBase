#include "BlockEventsHandler.h"
#include "BlockEvents/DisplayUpdateBlockEvent.h"
#include "spdlog/spdlog.h"

namespace PySysLinkBase
{
    void BlockEventsHandler::BlockEventCallback(const std::shared_ptr<BlockEvent> blockEvent) const
    {
        if (blockEvent->eventTypeId == "DisplayUpdate")
        {            
            std::shared_ptr<DisplayUpdateBlockEvent> displayUpdateBlockEvent = std::dynamic_pointer_cast<DisplayUpdateBlockEvent>(blockEvent);
                
            if (!displayUpdateBlockEvent) throw std::bad_cast();

            spdlog::get("default_pysyslink")->debug("Display {}, {:03.2f} s : {}", displayUpdateBlockEvent->displayId, displayUpdateBlockEvent->simulationTime, std::get<double>(displayUpdateBlockEvent->value));
        }
    }
} // namespace PySysLinkBase
