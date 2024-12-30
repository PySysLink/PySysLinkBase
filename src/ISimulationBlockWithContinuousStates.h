#ifndef PY_SYS_LINK_BASE_ISIMULATION_BLOCK_WITH_CONTINUOUS_STATES
#define PY_SYS_LINK_BASE_ISIMULATION_BLOCK_WITH_CONTINUOUS_STATES

#include "ISimulationBlock.h"
#include <vector>
#include <memory>
#include <stdexcept>

namespace PySysLinkBase
{
    class ISimulationBlockWithContinuousStates : public ISimulationBlock
    {
        public:
            virtual const std::vector<double> GetContinuousStates() const = 0;
            virtual void SetContinuousStates(std::vector<double> newStates) = 0;

            virtual const std::vector<double> GetContinousStateDerivatives(const std::shared_ptr<PySysLinkBase::SampleTime> sampleTime, double currentTime) const = 0;
            virtual const std::vector<std::vector<double>> GetContinuousStateJacobians(const std::shared_ptr<PySysLinkBase::SampleTime> sampleTime, double currentTime) const
            {
                throw std::logic_error("Jacobian not implemented in block " + this->GetId() + ". This is the deffault behaviour.");
            }

            virtual const std::vector<double> GetEvents(const std::shared_ptr<PySysLinkBase::SampleTime> sampleTime, double currentTime) const
            {
                return {};
            }
    };
} // namespace PySysLinkBase


#endif /* PY_SYS_LINK_BASE_ISIMULATION_BLOCK_WITH_CONTINUOUS_STATES */
