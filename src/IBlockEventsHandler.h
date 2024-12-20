#ifndef SRC_IBLOCK_EVENTS_HANDLER
#define SRC_IBLOCK_EVENTS_HANDLER

#include "BlockEvents/BlockEvent.h"
#include <memory>

namespace PySysLinkBase
{
    class IBlockEventsHandler
    {
        public:
        virtual ~IBlockEventsHandler() = default;
        
        virtual void BlockEventCallback(const std::shared_ptr<BlockEvent> blockEvent) const = 0;
    };
} // namespace PySysLinkBase



#endif /* SRC_IBLOCK_EVENTS_HANDLER */
