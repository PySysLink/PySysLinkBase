#include "Sumator.h"
#include <PySysLinkBase/PortsAndSignalValues/SignalValue.h>
#include <vector>

namespace BlockLibraries::BasicBlocks
{
    Sumator::Sumator(std::vector<double> gains)
    {
        this->gains = gains;
        this->sampleTimes.push_back(PySysLinkBase::SampleTime(PySysLinkBase::SampleTimeType::inherited, 
                                                            std::vector<PySysLinkBase::SampleTimeType>{PySysLinkBase::SampleTimeType::constant,
                                                                                                       PySysLinkBase::SampleTimeType::continuous,
                                                                                                       PySysLinkBase::SampleTimeType::discrete}));
                                                                                                       
        for (int i; i < this->gains.size(); i++)
        {
            std::unique_ptr<PySysLinkBase::UnknownTypeSignalValue> signalValue = std::make_unique<PySysLinkBase::SignalValue<double>>(PySysLinkBase::SignalValue<double>(0.0));
            auto inputPort = std::make_unique<PySysLinkBase::InputPort>(PySysLinkBase::InputPort(true, std::move(signalValue)));
            this->inputPorts.push_back(std::move(inputPort));
        }
        std::unique_ptr<PySysLinkBase::UnknownTypeSignalValue> signalValue = std::make_unique<PySysLinkBase::SignalValue<double>>(PySysLinkBase::SignalValue<double>(0.0));
        auto outputPort = std::make_unique<PySysLinkBase::OutputPort>(PySysLinkBase::OutputPort(std::move(signalValue)));
        this->outputPorts.push_back(std::move(outputPort));
    }

    const std::vector<PySysLinkBase::SampleTime>& Sumator::GetSampleTimes() const
    {
        return this->sampleTimes;
    }

    const std::vector<std::unique_ptr<PySysLinkBase::OutputPort>>& Sumator::ComputeOutputsOfBlock(PySysLinkBase::SampleTime sampleTime)
    {
        std::unique_ptr<PySysLinkBase::UnknownTypeSignalValue> outputValue = this->outputPorts[0]->GetValue();
        auto outputValueDouble = outputValue->TryCastToTyped<double>();
        double sum = 0.0;
        for (int i; i < this->gains.size(); i++)
        {
            auto inputValue = this->inputPorts[i]->GetValue();
            auto inputValueDouble = inputValue->TryCastToTyped<double>();
            sum += inputValueDouble->GetPayload() * this->gains[i];
        }
        outputValueDouble->SetPayload(sum);
        this->outputPorts[0]->SetValue(std::make_unique<PySysLinkBase::SignalValue<double>>(*outputValueDouble));
        return this->GetOutputPorts();
    }   
} // namespace BlockLibraries::BasicBlocks

