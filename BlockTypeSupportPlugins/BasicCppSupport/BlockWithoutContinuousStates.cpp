#include "BlockWithContinuousStates.h"
#include <stdexcept>

namespace BlockTypeSupports::BasicCppSupport
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
