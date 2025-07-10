#ifndef PYSYSLINK_BASE_SRC_CPP_LIBRARIES_PY_SYS_LINK_BASE_SRC_CONTINUOUS_AND_ODE_ISIMULATION_BLOCK_WITH_CONTINUOUS_STATES
#define PYSYSLINK_BASE_SRC_CPP_LIBRARIES_PY_SYS_LINK_BASE_SRC_CONTINUOUS_AND_ODE_ISIMULATION_BLOCK_WITH_CONTINUOUS_STATES

#include "../ISimulationBlock.h"
#include <vector>
#include <memory>
#include <stdexcept>
#include <utility>

namespace PySysLinkBase
{
    class ISimulationBlockWithContinuousStates : public ISimulationBlock
    {
        public:
            ISimulationBlockWithContinuousStates(std::map<std::string, ConfigurationValue> blockConfiguration, std::shared_ptr<IBlockEventsHandler> blockEventsHandler) 
                                                : ISimulationBlock(blockConfiguration, blockEventsHandler) {}

            virtual ~ISimulationBlockWithContinuousStates() = default;
                
            virtual const std::vector<double> GetContinuousStates() const = 0;
            virtual void SetContinuousStates(std::vector<double> newStates) = 0;

            virtual const std::vector<double> GetContinuousStateDerivatives(const std::shared_ptr<PySysLinkBase::SampleTime> sampleTime, double currentTime) const = 0;
    };
} // namespace PySysLinkBase


#endif /* PYSYSLINK_BASE_SRC_CPP_LIBRARIES_PY_SYS_LINK_BASE_SRC_CONTINUOUS_AND_ODE_ISIMULATION_BLOCK_WITH_CONTINUOUS_STATES */
