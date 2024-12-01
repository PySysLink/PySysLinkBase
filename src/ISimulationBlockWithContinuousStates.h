#ifndef PY_SYS_LINK_BASE_ISIMULATION_BLOCK_WITH_CONTINUOUS_STATES
#define PY_SYS_LINK_BASE_ISIMULATION_BLOCK_WITH_CONTINUOUS_STATES

#include "ISimulationBlock.h"
#include <vector>

namespace PySysLinkBase
{
    class ISimulationBlockWithContinuousStates : public ISimulationBlock
    {
        public:
            virtual const std::vector<ContinuousState>& GetContinuousStates() const;
            virtual void SetContinuousStates(std::vector<ContinuousState> newStates);

            virtual const std::vector<double>& GetContinousStateDerivatives() const;
    };
} // namespace PySysLinkBase


#endif /* PY_SYS_LINK_BASE_ISIMULATION_BLOCK_WITH_CONTINUOUS_STATES */
