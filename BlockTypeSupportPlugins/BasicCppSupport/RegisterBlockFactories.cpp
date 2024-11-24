#include <PySysLinkBase/IBlockFactory.h>
#include "BlockFactoryCpp.h"


extern "C" void RegisterBlockFactories(std::map<std::string, std::unique_ptr<PySysLinkBase::IBlockFactory>>& registry) {
    registry["BasicCpp"] = std::make_unique<BlockTypeSupports::BasicCppSupport::BlockFactoryCpp>();
}
