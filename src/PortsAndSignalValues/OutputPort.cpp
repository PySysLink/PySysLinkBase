#include "OutputPort.h"

namespace PySysLinkBase
{
    OutputPort::OutputPort(std::shared_ptr<UnknownTypeSignalValue> value) : Port(value)
    {

    }

    std::shared_ptr<OutputPort> CreateOutputPortOfType(PortTypeMetadata portType)
    {
        if (portType.category == PortCategory::FullySupportedSignalValue)
        {
            if (!portType.signalValueType.has_value())
            {
                throw std::invalid_argument("Output port type is FullySupportedSignalValue but no signal value type is provided.");
            }
            return DispatchType(portType.signalValueType.value(), [&]<typename O>()
            {
                return std::make_shared<OutputPort>(OutputPort(std::make_shared<SignalValue<O>>(SignalValue<O>(O{}))));
            });
        }
        else
        {
            throw std::invalid_argument("Output port type is not FullySupportedSignalValue. Only FullySupportedSignalValue is supported in this implementation.");
        }
    }
} // namespace PySysLinkBase
