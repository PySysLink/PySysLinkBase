#include "ContinuousState.h"

namespace PySysLinkBase
{
    ContinuousState::ContinuousState(double initialValue)
    {
        this->value = initialValue;
    }
    ContinuousState::ContinuousState()
    {
        ContinuousState(0);
    }

    double ContinuousState::GetValue() const
    {
        return this->value;
    }
    void ContinuousState::SetValue(double newValue)
    {
        this->value = newValue;
    }
}