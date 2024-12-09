#include "InputPort.h"
#include "ISimulationBlock.h"

namespace PySysLinkBase
{
    InputPort::InputPort(bool hasDirectFeedthrough, std::unique_ptr<UnknownTypeSignalValue> value) : Port(std::move(value))
    {
        this->hasDirectFeedthrough = hasDirectFeedthrough;
    }

    const bool InputPort::HasDirectFeedtrough() const
    {
        return this->hasDirectFeedthrough;
    }   

} // namespace PySysLinkBase
