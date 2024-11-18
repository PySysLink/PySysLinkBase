#include <PySysLinkBase/SimulationModel.h>
#include <PySysLinkBase/ISimulationBlock.h>
#include <PySysLinkBase/PortLink.h>
#include <iostream>

int main() {

    std::vector<std::unique_ptr<PySysLinkBase::ISimulationBlock>> simulationBlocks = {};

    std::vector<std::unique_ptr<PySysLinkBase::PortLink>> portLinks = {};

    std::unique_ptr<PySysLinkBase::SimulationModel> model =
        std::make_unique<PySysLinkBase::SimulationModel>(
            std::move(simulationBlocks),
            std::move(portLinks)
        );    
    std::cout << "End" << std::endl;
    return 0;
}