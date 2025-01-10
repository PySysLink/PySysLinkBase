#ifndef SRC_BLOCK_EVENTS_HANDLER
#define SRC_BLOCK_EVENTS_HANDLER

#include "BlockEvents/BlockEvent.h"
#include "BlockEvents/ValueUpdateBlockEvent.h"
#include "IBlockEventsHandler.h"
#include <memory>
#include <functional>

namespace PySysLinkBase
{
    class BlockEventsHandler : public IBlockEventsHandler
    {
        private:
        std::vector<std::function<void (std::shared_ptr<ValueUpdateBlockEvent>)>> valueUpdateBlockEventCallbacks;

        public:

        BlockEventsHandler();

        void BlockEventCallback(const std::shared_ptr<BlockEvent> blockEvent) const;

        void RegisterValueUpdateBlockEventCallback(std::function<void (std::shared_ptr<ValueUpdateBlockEvent>)> callback);
    };
} // namespace PySysLinkBase

#endif /* SRC_BLOCK_EVENTS_HANDLER */
