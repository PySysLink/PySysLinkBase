#include "OutputPort.h"

namespace PySysLinkBase
{
    OutputPort::OutputPort(std::unique_ptr<UnknownTypeSignalValue> value, ISimulationBlock& ownerBlock) : Port(std::move(value), ownerBlock)
    {

    }
} // namespace PySysLinkBase
