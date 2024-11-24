#ifndef SRC_BLOCK_TYPE_SUPPORTS_BASIC_CPP_SUPPORT_BLOCK_FACTORY_CPP
#define SRC_BLOCK_TYPE_SUPPORTS_BASIC_CPP_SUPPORT_BLOCK_FACTORY_CPP

#include <PySysLinkBase/IBlockFactory.h>

namespace BlockTypeSupports::BasicCppSupport
{
    class BlockFactoryCpp : public PySysLinkBase::IBlockFactory
    {
        public:
            BlockFactoryCpp();
            // virtual ~BlockFactoryCpp();
            std::unique_ptr<PySysLinkBase::ISimulationBlock> CreateBlock(std::map<std::string, PySysLinkBase::ConfigurationValue> blockConfiguration);
    };
} // namespace BlockTypeSupports::BasicCppSupport


#endif /* SRC_BLOCK_TYPE_SUPPORTS_BASIC_CPP_SUPPORT_BLOCK_FACTORY_CPP */
