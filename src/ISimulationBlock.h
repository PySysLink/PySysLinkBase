#ifndef SRC_PY_SYS_LINK_BASE_ISIMULATION_BLOCK
#define SRC_PY_SYS_LINK_BASE_ISIMULATION_BLOCK

#include <string>
#include <vector>
#include <memory>
#include "PortsAndSignalValues/InputPort.h"
#include "PortsAndSignalValues/OutputPort.h"
#include "SampleTime.h"
#include "ContinuousState.h"
#include <stdexcept>
#include <map>
#include "ConfigurationValue.h"

namespace PySysLinkBase
{
    class ISimulationBlock {
    protected:
        std::string name;
        std::string id;
    public:
        const std::string GetId() const;
        const std::string GetName() const;

        ISimulationBlock(std::map<std::string, ConfigurationValue> blockConfiguration);
        virtual ~ISimulationBlock() = default;

        virtual const std::vector<SampleTime>& GetSampleTimes() const = 0;

        virtual std::vector<std::unique_ptr<PySysLinkBase::InputPort>>& GetInputPorts() = 0;
        virtual const std::vector<std::unique_ptr<PySysLinkBase::OutputPort>>& GetOutputPorts() const = 0;

        virtual const std::vector<std::unique_ptr<PySysLinkBase::OutputPort>>& ComputeOutputsOfBlock(PySysLinkBase::SampleTime sampleTime) = 0;

        static ISimulationBlock& FindBlockById(std::string id, const std::vector<std::unique_ptr<ISimulationBlock>>& blocksToFind);
    };
}

#endif /* SRC_PY_SYS_LINK_BASE_ISIMULATION_BLOCK */
