#ifndef SRC_BLOCK_EVENTS_BLOCK_EVENT
#define SRC_BLOCK_EVENTS_BLOCK_EVENT

#include <string>

namespace PySysLinkBase
{
    class BlockEvent
    {
        public:
        std::string eventTypeId;

        BlockEvent(std::string eventTypeId) : eventTypeId(eventTypeId) {}

        virtual ~BlockEvent() = default; // Ensures the class is polymorphic
    };
} // namespace PySysLinkBase


#endif /* SRC_BLOCK_EVENTS_BLOCK_EVENT */
