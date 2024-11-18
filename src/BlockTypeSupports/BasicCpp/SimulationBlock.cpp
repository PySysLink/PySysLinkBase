#include "SimulationBlock.h"
namespace BlockTypeSupports::BasicCpp
{
    std::vector<std::unique_ptr<PySysLinkBase::InputPort>>& SimulationBlock::GetInputPorts()
    {
        return this->inputPorts;
    }
            
    const std::vector<std::unique_ptr<PySysLinkBase::OutputPort>>& SimulationBlock::GetOutputPorts() const
    {
        return this->outputPorts;
    }
}
