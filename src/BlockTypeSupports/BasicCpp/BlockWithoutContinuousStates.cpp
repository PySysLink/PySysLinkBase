#include "BlockWithContinuousStates.h"
#include <stdexcept>

namespace BlockTypeSupports::BasicCpp
{
    const std::vector<PySysLinkBase::ContinuousState>& SimulationBlockWithContinuousStates::GetContinuousStates() const
    {
        return this->ContinuousStates;
    }
 
    void SimulationBlockWithContinuousStates::SetContinuousStates(std::vector<PySysLinkBase::ContinuousState> newStates)
    {
        this->ContinuousStates = newStates;
    }

}
