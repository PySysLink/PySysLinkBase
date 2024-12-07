#include "InputPort.h"
#include "ISimulationBlock.h"

namespace PySysLinkBase
{
    InputPort::InputPort(bool hasDirectFeedthrough, std::unique_ptr<UnknownTypeSignalValue> value, ISimulationBlock& ownerBlock) : Port(std::move(value), ownerBlock)
    {
        this->hasDirectFeedthrough = hasDirectFeedthrough;
    }

    const bool InputPort::HasDirectFeedtrough() const
    {
        return this->hasDirectFeedthrough;
    }

    bool InputPort::IsInputDirectBlockChainEnd() const
    {
        if (this->ownerBlock.GetOutputPorts().size() == 0 || !this->hasDirectFeedthrough)
        {
            return true;
        }
        else
        {
            return false;
        }
    }    

} // namespace PySysLinkBase
