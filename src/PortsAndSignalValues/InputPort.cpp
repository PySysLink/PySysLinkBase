#include "InputPort.h"
#include "ISimulationBlock.h"

namespace PySysLinkBase
{
    InputPort::InputPort(bool hasDirectFeedthrough, std::shared_ptr<UnknownTypeSignalValue> value) : Port(value)
    {
        this->hasDirectFeedthrough = hasDirectFeedthrough;
    }

    const bool InputPort::HasDirectFeedtrough() const
    {
        return this->hasDirectFeedthrough;
    }   

} // namespace PySysLinkBase
