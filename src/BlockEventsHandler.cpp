#include "BlockEventsHandler.h"
#include "BlockEvents/ValueUpdateBlockEvent.h"
#include "spdlog/spdlog.h"
#include <sstream>

namespace PySysLinkBase
{
    BlockEventsHandler::BlockEventsHandler()
    {

    }

    void BlockEventsHandler::BlockEventCallback(const std::shared_ptr<BlockEvent> blockEvent) const
    {
        if (blockEvent->eventTypeId == "ValueUpdate")
        {            
            std::shared_ptr<ValueUpdateBlockEvent> displayUpdateBlockEvent = std::dynamic_pointer_cast<ValueUpdateBlockEvent>(blockEvent);
                
            if (!displayUpdateBlockEvent) throw std::bad_cast();

            try
            {
                spdlog::get("default_pysyslink")->info("Value {}, {:03.2f} s : {}", displayUpdateBlockEvent->valueId, displayUpdateBlockEvent->simulationTime, std::get<double>(displayUpdateBlockEvent->value));
            }
            catch(const std::exception& e)
            {
                std::ostringstream oss;
                oss << std::get<std::complex<double>>(displayUpdateBlockEvent->value);
                std::string complexNumber = oss.str();
                spdlog::get("default_pysyslink")->info("Value {}, {:03.2f} s : {}", displayUpdateBlockEvent->valueId, displayUpdateBlockEvent->simulationTime, complexNumber);
            }
            
        }
    }
} // namespace PySysLinkBase
