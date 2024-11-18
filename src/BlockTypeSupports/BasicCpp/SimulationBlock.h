#ifndef BLOCK_TYPE_SUPPORTS_BASIC_CPP_SIMULATION_BLOCK
#define BLOCK_TYPE_SUPPORTS_BASIC_CPP_SIMULATION_BLOCK


#include <string>
#include <vector>
#include <memory>
#include <PySysLinkBase/PortsAndSignalValues/InputPort.h>
#include <PySysLinkBase/PortsAndSignalValues/OutputPort.h>
#include <PySysLinkBase/SampleTime.h>
#include <PySysLinkBase/ContinuousState.h>
#include <PySysLinkBase/ISimulationBlock.h>

namespace BlockTypeSupports::BasicCpp
{
    class SimulationBlock : public PySysLinkBase::ISimulationBlock {
        protected:
            std::vector<std::unique_ptr<PySysLinkBase::InputPort>> inputPorts;
            std::vector<std::unique_ptr<PySysLinkBase::OutputPort>> outputPorts;

        public:
            virtual const std::vector<PySysLinkBase::SampleTime>& GetSampleTimes() const;

            std::vector<std::unique_ptr<PySysLinkBase::InputPort>>& GetInputPorts();
            const std::vector<std::unique_ptr<PySysLinkBase::OutputPort>>& GetOutputPorts() const;

            virtual const std::vector<std::unique_ptr<PySysLinkBase::OutputPort>>& ComputeOutputsOfBlock(PySysLinkBase::SampleTime sampleTime);
    };
}

#endif /* BLOCK_TYPE_SUPPORTS_BASIC_CPP_SIMULATION_BLOCK */
