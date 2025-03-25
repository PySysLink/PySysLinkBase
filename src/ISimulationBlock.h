#ifndef SRC_ISIMULATION_BLOCK
#define SRC_ISIMULATION_BLOCK

#include <string>
#include <vector>
#include <memory>
#include "PortsAndSignalValues/InputPort.h"
#include "PortsAndSignalValues/OutputPort.h"
#include "SampleTime.h"
#include <stdexcept>
#include <map>
#include "ConfigurationValue.h"
#include "BlockEvents/BlockEvent.h"
#include "IBlockEventsHandler.h"

namespace PySysLinkBase
{
    class ISimulationBlock {        
    protected:
        std::shared_ptr<IBlockEventsHandler> blockEventsHandler;
        
        std::string name;
        std::string id;

        std::vector<std::function<void (const std::string, const std::vector<std::shared_ptr<PySysLinkBase::InputPort>>, std::shared_ptr<PySysLinkBase::SampleTime>, double)>> readInputCallbacks;
        std::vector<std::function<void (const std::string, const std::vector<std::shared_ptr<PySysLinkBase::OutputPort>>, std::shared_ptr<PySysLinkBase::SampleTime>, double)>> calculateOutputCallbacks;
        std::vector<std::function<void (const std::string, const std::string, const ConfigurationValue)>> updateConfigurationValueCallbacks;
    public:
        const std::string GetId() const;
        const std::string GetName() const;

        ISimulationBlock(std::map<std::string, ConfigurationValue> blockConfiguration, std::shared_ptr<IBlockEventsHandler> blockEventsHandler);
        virtual ~ISimulationBlock() = default;

        const virtual std::shared_ptr<SampleTime> GetSampleTime() const = 0;
        virtual void SetSampleTime(std::shared_ptr<SampleTime> sampleTime) = 0;

        virtual std::vector<std::shared_ptr<PySysLinkBase::InputPort>> GetInputPorts() const = 0;
        virtual const std::vector<std::shared_ptr<PySysLinkBase::OutputPort>> GetOutputPorts() const = 0;

        const std::vector<std::shared_ptr<PySysLinkBase::OutputPort>> ComputeOutputsOfBlock(const std::shared_ptr<PySysLinkBase::SampleTime> sampleTime, double currentTime, bool isMinorStep=false);
        virtual const std::vector<std::shared_ptr<PySysLinkBase::OutputPort>> _ComputeOutputsOfBlock(const std::shared_ptr<PySysLinkBase::SampleTime> sampleTime, double currentTime, bool isMinorStep=false) = 0;

        bool IsBlockFreeSource() const;
        bool IsInputDirectBlockChainEnd(int inputIndex) const;

        void NotifyEvent(std::shared_ptr<PySysLinkBase::BlockEvent> blockEvent) const;
        bool TryUpdateConfigurationValue(std::string keyName, ConfigurationValue value);
        virtual bool _TryUpdateConfigurationValue(std::string keyName, ConfigurationValue value) = 0;

        static std::shared_ptr<ISimulationBlock> FindBlockById(std::string id, const std::vector<std::shared_ptr<ISimulationBlock>>& blocksToFind);

        void RegisterReadInputsCallbacks(std::function<void (const std::string, const std::vector<std::shared_ptr<PySysLinkBase::InputPort>>, std::shared_ptr<PySysLinkBase::SampleTime>, double)> callback);
        void RegisterCalculateOutputCallbacks(std::function<void (const std::string, const std::vector<std::shared_ptr<PySysLinkBase::OutputPort>>, std::shared_ptr<PySysLinkBase::SampleTime>, double)> callback);
        void RegisterUpdateConfigurationValueCallbacks(std::function<void (const std::string, const std::string, const ConfigurationValue)> callback);

        virtual const std::vector<std::pair<double, double>> GetEvents(const std::shared_ptr<PySysLinkBase::SampleTime> sampleTime, double eventTime, std::vector<double> eventTimeStates, bool includeKnownEvents=false) const
        {
            return {};
        }

        virtual const std::vector<double> GetKnownEvents(const std::shared_ptr<PySysLinkBase::SampleTime> resolvedSampleTime, double simulationStartTime, double simulationEndTime) const
        {
            return {};
        }
    };
}

#endif /* SRC_ISIMULATION_BLOCK */
