#include <PySysLinkBase/SimulationModel.h>
#include <PySysLinkBase/ModelParser.h>
#include <PySysLinkBase/ISimulationBlock.h>
#include <PySysLinkBase/PortLink.h>
#include <PySysLinkBase/IBlockFactory.h>
#include <PySysLinkBase/BlockTypeSupportPlugingLoader.h>
#include <BlockTypeSupports/BasicCppSupport/BlockFactoryCpp.h>
#include <iostream>
#include <map>

int main() {
    std::unique_ptr<PySysLinkBase::BlockTypeSupportPlugingLoader> plugingLoader = std::make_unique<PySysLinkBase::BlockTypeSupportPlugingLoader>();
    plugingLoader->LoadPlugins("dummy_path");
    
    std::map<std::string, std::unique_ptr<PySysLinkBase::IBlockFactory>> blockFactories;
    blockFactories.emplace(
        "BasicCpp", std::make_unique<BlockTypeSupports::BasicCppSupport::BlockFactoryCpp>()
    );

    PySysLinkBase::SimulationModel simulationModel = PySysLinkBase::ModelParser::ParseFromYaml("/home/pello/PySysLink/Tests/system1.yaml", blockFactories);

    
    return 0;
}

