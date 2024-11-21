#ifndef PY_SYS_LINK_BASE_IBLOCK_FACTORY
#define PY_SYS_LINK_BASE_IBLOCK_FACTORY

#include "ISimulationBlock.h"
#include <vector>
#include "ConfigurationValue.h"
#include <map>
#include <string>

namespace PySysLinkBase
{
   class IBlockFactory {
      public:
         virtual ISimulationBlock CreateBlock(std::vector<std::map<std::string, ConfigurationValue>> blockConfiguration) = 0;
   };
}

#endif /* PY_SYS_LINK_BASE_IBLOCK_FACTORY */
