// DummySimulationBlock.h
#ifndef DUMMY_SIMULATION_BLOCK_H
#define DUMMY_SIMULATION_BLOCK_H

#include "PySysLinkBase/ISimulationBlock.h"
#include "PySysLinkBase/SampleTime.h"
#include "PySysLinkBase/PortsAndSignalValues/InputPort.h"
#include "PySysLinkBase/PortsAndSignalValues/OutputPort.h"
#include "PySysLinkBase/PortsAndSignalValues/SignalValue.h"
#include <memory>
#include <map>
#include <string>
#include <vector>

class DummySimulationBlock : public PySysLinkBase::ISimulationBlock {
private:
    // Dummy simulation block implementation
    std::vector<std::shared_ptr<PySysLinkBase::InputPort>> inputPorts;
    std::vector<std::shared_ptr<PySysLinkBase::OutputPort>> outputPorts;

public:
    DummySimulationBlock(const std::map<std::string, PySysLinkBase::ConfigurationValue>& conf,
                         std::shared_ptr<PySysLinkBase::IBlockEventsHandler> handler, int inputPortAmount = 1, int outputPortAmount = 1)
        : ISimulationBlock(conf, handler) 
    {
        for (int i = 0; i < inputPortAmount; i++)
        {
            std::shared_ptr<PySysLinkBase::UnknownTypeSignalValue> signalValue = std::make_shared<PySysLinkBase::SignalValue<double>>(PySysLinkBase::SignalValue<double>(0.0));
            auto inputPort = std::make_shared<PySysLinkBase::InputPort>(PySysLinkBase::InputPort(true, signalValue));
            this->inputPorts.push_back(inputPort);
        }
        for (int i = 0; i < outputPortAmount; i++)
        {
            std::shared_ptr<PySysLinkBase::UnknownTypeSignalValue> signalValue = std::make_shared<PySysLinkBase::SignalValue<double>>(PySysLinkBase::SignalValue<double>(0.0));
            auto outputPort = std::make_shared<PySysLinkBase::OutputPort>(PySysLinkBase::OutputPort(signalValue));
            this->outputPorts.push_back(outputPort);
        }
    }

    // Provide dummy implementations for the pure virtual functions.
    const std::shared_ptr<PySysLinkBase::SampleTime> GetSampleTime() const override {
        // Return a valid (even if dummy) sample time pointer
        return std::make_shared<PySysLinkBase::SampleTime>(PySysLinkBase::SampleTimeType::discrete, 1.0);
    }
    void SetSampleTime(std::shared_ptr<PySysLinkBase::SampleTime> sampleTime) override {
        // Optionally store sampleTime if needed for further tests
    }
    std::vector<std::shared_ptr<PySysLinkBase::InputPort>> GetInputPorts() const override {
        return {this->inputPorts}; 
    }
    const std::vector<std::shared_ptr<PySysLinkBase::OutputPort>> GetOutputPorts() const override {
        return {this->outputPorts}; 
    }
    const std::vector<std::shared_ptr<PySysLinkBase::OutputPort>> _ComputeOutputsOfBlock(
            std::shared_ptr<PySysLinkBase::SampleTime> sampleTime, double currentTime, bool isMinorStep=false) override {
        return {this->outputPorts}; 
    }
    bool _TryUpdateConfigurationValue(std::string keyName, PySysLinkBase::ConfigurationValue value) override {
        return true; // always succeed in updating
    }
};

#endif /* DUMMY_SIMULATION_BLOCK_H */
