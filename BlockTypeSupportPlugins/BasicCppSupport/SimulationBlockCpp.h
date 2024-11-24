#ifndef SRC_BLOCK_TYPE_SUPPORTS_BASIC_CPP_SUPPORT_SIMULATION_BLOCK_CPP
#define SRC_BLOCK_TYPE_SUPPORTS_BASIC_CPP_SUPPORT_SIMULATION_BLOCK_CPP

#include <string>
#include <vector>
#include <memory>
#include <PySysLinkBase/PortsAndSignalValues/InputPort.h>
#include <PySysLinkBase/PortsAndSignalValues/OutputPort.h>
#include <PySysLinkBase/SampleTime.h>
#include <PySysLinkBase/ContinuousState.h>
#include <PySysLinkBase/ISimulationBlock.h>
#include <BlockTypes/BasicCpp/SimulationBlock.h>
#include <BlockTypes/BasicCpp/SampleTime.h>


namespace BlockTypeSupports::BasicCppSupport
{
    class SimulationBlockCpp : public PySysLinkBase::ISimulationBlock {
        private:
            std::unique_ptr<BlockTypes::BasicCpp::SimulationBlock> simulationBlock;
            std::vector<PySysLinkBase::SampleTime> sampleTimes;
            std::string id;
            std::string name;
        protected:
            std::vector<std::unique_ptr<PySysLinkBase::InputPort>> inputPorts;
            std::vector<std::unique_ptr<PySysLinkBase::OutputPort>> outputPorts;

        public:
            const std::string GetId() const;
            const std::string GetName() const;

            SimulationBlockCpp(std::unique_ptr<BlockTypes::BasicCpp::SimulationBlock> simulationBlock, std::map<std::string, PySysLinkBase::ConfigurationValue> blockConfiguration);
            const std::vector<PySysLinkBase::SampleTime>& GetSampleTimes() const;

            std::vector<std::unique_ptr<PySysLinkBase::InputPort>>& GetInputPorts();
            const std::vector<std::unique_ptr<PySysLinkBase::OutputPort>>& GetOutputPorts() const;

            const std::vector<std::unique_ptr<PySysLinkBase::OutputPort>>& ComputeOutputsOfBlock(PySysLinkBase::SampleTime sampleTime);
    };
}

#endif /* SRC_BLOCK_TYPE_SUPPORTS_BASIC_CPP_SUPPORT_SIMULATION_BLOCK_CPP */
