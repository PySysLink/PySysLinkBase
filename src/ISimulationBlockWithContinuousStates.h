#ifndef PY_SYS_LINK_BASE_ISIMULATION_BLOCK_WITH_CONTINUOUS_STATES
#define PY_SYS_LINK_BASE_ISIMULATION_BLOCK_WITH_CONTINUOUS_STATES

#include "ISimulationBlock.h"
#include <vector>
#include <memory>

namespace PySysLinkBase
{
    class ISimulationBlockWithContinuousStates : public ISimulationBlock
    {
        public:
            virtual const std::vector<double> GetContinuousStates() const;
            virtual void SetContinuousStates(std::vector<double> newStates);

            virtual const std::vector<double> GetContinousStateDerivatives() const;
    };
} // namespace PySysLinkBase


#endif /* PY_SYS_LINK_BASE_ISIMULATION_BLOCK_WITH_CONTINUOUS_STATES */
