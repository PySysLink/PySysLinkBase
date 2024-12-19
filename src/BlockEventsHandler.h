#ifndef SRC_BLOCK_EVENTS_HANDLER
#define SRC_BLOCK_EVENTS_HANDLER

#include "BlockEvents/BlockEvent.h"
#include <memory>

namespace PySysLinkBase
{
    class BlockEventsHandler
    {
        public:

        BlockEventsHandler();

        void BlockEventCallback(const std::shared_ptr<BlockEvent> blockEvent) const;
    };
} // namespace PySysLinkBase

#endif /* SRC_BLOCK_EVENTS_HANDLER */
