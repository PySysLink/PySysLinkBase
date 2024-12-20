#ifndef PY_SYS_LINK_BASE_IBLOCK_FACTORY
#define PY_SYS_LINK_BASE_IBLOCK_FACTORY

#include "ISimulationBlock.h"
#include <vector>
#include "ConfigurationValue.h"
#include <map>
#include <memory>
#include <string>
#include "IBlockEventsHandler.h"

namespace PySysLinkBase
{
   class IBlockFactory {
      public:
         virtual ~IBlockFactory() = default;
         virtual std::unique_ptr<ISimulationBlock> CreateBlock(std::map<std::string, ConfigurationValue> blockConfiguration, std::shared_ptr<IBlockEventsHandler> blockEventsHandler) = 0;
   };
}

#endif /* PY_SYS_LINK_BASE_IBLOCK_FACTORY */
