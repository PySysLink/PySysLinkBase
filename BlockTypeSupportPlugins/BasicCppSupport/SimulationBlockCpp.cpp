#include "SimulationBlockCpp.h"
#include <PySysLinkBase/PortsAndSignalValues/SignalValue.h>
#include <stdexcept>
#include "SampleTimeConversion.h"
#include <iostream>

namespace BlockTypeSupports::BasicCppSupport
{
    SimulationBlockCpp::SimulationBlockCpp(std::unique_ptr<BlockTypes::BasicCpp::SimulationBlock> simulationBlock, std::map<std::string, PySysLinkBase::ConfigurationValue> blockConfiguration) : ISimulationBlock(blockConfiguration)
    {
        std::cout << "Creating basic simulation block cpp..." << std::endl;
        this->simulationBlock = std::move(simulationBlock);

        std::cout << "Simulation block moved..." << std::endl;

        std::vector<bool> inputsHasDirectFeedthrough = this->simulationBlock->InputsHasDirectFeedthrough();
        if (inputsHasDirectFeedthrough.size() != this->simulationBlock->GetInputPortAmmount()) {
            throw std::runtime_error("Mismatch between the number of input ports and the size of the inputsHasDirectFeedthrough vector");
        }
        for (int i = 0; i < this->simulationBlock->GetInputPortAmmount(); i++)
        {
            std::unique_ptr<PySysLinkBase::UnknownTypeSignalValue> signalValue = std::make_unique<PySysLinkBase::SignalValue<double>>(PySysLinkBase::SignalValue<double>(0.0));
            auto inputPort = std::make_unique<PySysLinkBase::InputPort>(PySysLinkBase::InputPort(inputsHasDirectFeedthrough[i], std::move(signalValue)));
            this->inputPorts.push_back(std::move(inputPort));
        }
        for (int i = 0; i < this->simulationBlock->GetOutputPortAmmount(); i++)
        {
            std::unique_ptr<PySysLinkBase::UnknownTypeSignalValue> signalValue = std::make_unique<PySysLinkBase::SignalValue<double>>(PySysLinkBase::SignalValue<double>(0.0));
            auto outputPort = std::make_unique<PySysLinkBase::OutputPort>(PySysLinkBase::OutputPort(std::move(signalValue)));
            this->outputPorts.push_back(std::move(outputPort));
        }

        std::cout << "Ports configured..." << std::endl;

        std::vector<BlockTypes::BasicCpp::SampleTime> sampleTimesCpp = this->simulationBlock->GetSampleTimes();

        std::cout << "Basic simulation block cpp created" << std::endl;
    }

    const std::vector<PySysLinkBase::SampleTime>& SimulationBlockCpp::GetSampleTimes() const
    {
        return this->sampleTimes;
    }

    std::vector<std::unique_ptr<PySysLinkBase::InputPort>>& SimulationBlockCpp::GetInputPorts()
    {
        return this->inputPorts;
    }
            
    const std::vector<std::unique_ptr<PySysLinkBase::OutputPort>>& SimulationBlockCpp::GetOutputPorts() const
    {
        return this->outputPorts;
    }

    const std::vector<std::unique_ptr<PySysLinkBase::OutputPort>>& SimulationBlockCpp::ComputeOutputsOfBlock(PySysLinkBase::SampleTime sampleTime)
    {
        std::vector<double> inputValues = {};
        for (int i; i < this->simulationBlock->GetInputPortAmmount(); i++)
        {
            auto inputValue = this->inputPorts[i]->GetValue();
            auto inputValueDouble = inputValue->TryCastToTyped<double>();
            inputValues.push_back(inputValueDouble->GetPayload());
        }
        std::vector<double> outputValues = this->simulationBlock->CalculateOutputs(inputValues, SampleTimeConversion::PySysLinkTimeToCpp(sampleTime));
        
        for (int i; i < this->simulationBlock->GetOutputPortAmmount(); i++)
        {
            std::unique_ptr<PySysLinkBase::UnknownTypeSignalValue> outputValue = this->outputPorts[i]->GetValue();
            auto outputValueDouble = outputValue->TryCastToTyped<double>();
            outputValueDouble->SetPayload(outputValues[i]);
            this->outputPorts[i]->SetValue(std::make_unique<PySysLinkBase::SignalValue<double>>(*outputValueDouble));
        }
        
        return this->GetOutputPorts();
    }
}
