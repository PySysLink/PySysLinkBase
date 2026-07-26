#include "BlockEventsHandler.h"
#include "BlockEvents/ValueUpdateBlockEvent.h"
#include "spdlog/spdlog.h"
#include <sstream>
#include "FullySupportedSignalValue.h"

namespace PySysLinkBase
{
    BlockEventsHandler::BlockEventsHandler()
    {
        this->valueUpdateBlockEventCallbacks = {};
    }

    void BlockEventsHandler::BlockEventCallback(const std::shared_ptr<BlockEvent> blockEvent) const
    {
        if (blockEvent->eventTypeId == "ValueUpdate")
        {            
            std::shared_ptr<ValueUpdateBlockEvent> displayUpdateBlockEvent = std::dynamic_pointer_cast<ValueUpdateBlockEvent>(blockEvent);
                
            if (!displayUpdateBlockEvent) throw std::bad_cast();

            spdlog::get("default_pysyslink")->info("Value {}, {:03.2f} s : {}", displayUpdateBlockEvent->valueId, displayUpdateBlockEvent->simulationTime, 
                                                                                FullySupportedSignalValueToString(displayUpdateBlockEvent->value));

            for (const auto& callback : this->valueUpdateBlockEventCallbacks)
            {
                callback(displayUpdateBlockEvent);
            }
            
        }
    }

    void BlockEventsHandler::RegisterValueUpdateBlockEventCallback(std::function<void (std::shared_ptr<ValueUpdateBlockEvent>)> callback)
    {
        this->valueUpdateBlockEventCallbacks.push_back(callback);
    }

} // namespace PySysLinkBase
