#include "InputPort.h"

namespace PySysLinkBase
{
    InputPort::InputPort(bool hasDirectFeedthrough, std::shared_ptr<UnknownTypeSignalValue> value) : Port(value)
    {
        this->hasDirectFeedthrough = hasDirectFeedthrough;
    }

    const bool InputPort::HasDirectFeedthrough() const
    {
        return this->hasDirectFeedthrough;
    }   

    std::shared_ptr<InputPort> CreateInputPortOfType(PortTypeMetadata portType, bool hasDirectFeedthrough)
    {
        if (portType.category == PortCategory::FullySupportedSignalValue)
        {
            if (!portType.signalValueType.has_value())
            {
                throw std::invalid_argument("Input port type is FullySupportedSignalValue but no signal value type is provided.");
            }
            return DispatchType(portType.signalValueType.value(), [&]<typename I>()
            {
                return std::make_shared<InputPort>(InputPort(hasDirectFeedthrough, std::make_shared<SignalValue<I>>(SignalValue<I>(I{}))));
            });
        }
        else
        {
            throw std::invalid_argument("Input port type is not FullySupportedSignalValue. Only FullySupportedSignalValue is supported in this implementation.");
        }
    }

} // namespace PySysLinkBase
