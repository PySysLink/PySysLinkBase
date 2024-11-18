#include "Port.h"
#include <typeinfo>
#include <stdexcept>

namespace PySysLinkBase
{
    Port::Port(std::unique_ptr<UnknownTypeSignalValue> value)
    {
        this->value = std::move(value);
    }

    void Port::TryCopyValueToPort(Port &otherPort) const
    {
        if (this->GetValue()->GetTypeId() == otherPort.GetValue()->GetTypeId())
        {
            otherPort.SetValue(this->GetValue()->clone());
        }
        else
        {
            throw std::bad_cast();
        }
    } 

    void Port::SetValue(std::unique_ptr<UnknownTypeSignalValue> value)
    {
        this->value = move(value);
    }
    std::unique_ptr<UnknownTypeSignalValue> Port::GetValue() const
    {
        if (!this->value)
        {
            throw std::runtime_error("Value was null, should not be");
        }
        return this->value->clone();
    }
} // namespace PySysLinkBase
