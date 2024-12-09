#include "OutputPort.h"

namespace PySysLinkBase
{
    OutputPort::OutputPort(std::unique_ptr<UnknownTypeSignalValue> value) : Port(std::move(value))
    {

    }
} // namespace PySysLinkBase
