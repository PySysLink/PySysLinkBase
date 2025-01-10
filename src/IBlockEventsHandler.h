#ifndef SRC_IBLOCK_EVENTS_HANDLER
#define SRC_IBLOCK_EVENTS_HANDLER

#include "BlockEvents/BlockEvent.h"
#include "BlockEvents/ValueUpdateBlockEvent.h"
#include <memory>
#include <functional>

namespace PySysLinkBase
{
    class IBlockEventsHandler
    {
        public:
        virtual ~IBlockEventsHandler() = default;
        
        virtual void BlockEventCallback(const std::shared_ptr<BlockEvent> blockEvent) const = 0;
        virtual void RegisterValueUpdateBlockEventCallback(std::function<void (std::shared_ptr<ValueUpdateBlockEvent>)> callback) = 0;

    };
} // namespace PySysLinkBase



#endif /* SRC_IBLOCK_EVENTS_HANDLER */
