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

        virtual std::shared_ptr<SampleTime> GetSampleTime() = 0;
        virtual void SetSampleTime(std::shared_ptr<SampleTime> sampleTime) = 0;

        virtual std::vector<std::shared_ptr<PySysLinkBase::InputPort>> GetInputPorts() const = 0;
        virtual const std::vector<std::shared_ptr<PySysLinkBase::OutputPort>> GetOutputPorts() const = 0;

        virtual const std::vector<std::shared_ptr<PySysLinkBase::OutputPort>> ComputeOutputsOfBlock(const std::shared_ptr<PySysLinkBase::SampleTime> sampleTime) = 0;

        static ISimulationBlock& FindBlockById(std::string id, const std::vector<std::unique_ptr<ISimulationBlock>>& blocksToFind);
        static std::shared_ptr<ISimulationBlock> FindBlockById(std::string id, const std::vector<std::shared_ptr<ISimulationBlock>>& blocksToFind);

        bool IsBlockFreeSource() const;

        bool IsInputDirectBlockChainEnd(int inputIndex) const;
    };
}

#endif /* SRC_PY_SYS_LINK_BASE_ISIMULATION_BLOCK */
