#include "Constant.h"
#include <PySysLinkBase/PortsAndSignalValues/SignalValue.h>
#include <PySysLinkBase/PortsAndSignalValues/UnknownTypeSignalValue.h>
#include <vector>

namespace BlockLibraries::BasicBlocks
{
    Constant::Constant(double value)
    {
        this->value = value;
        this->sampleTimes.push_back(PySysLinkBase::SampleTime(PySysLinkBase::SampleTimeType::constant));
        std::unique_ptr<PySysLinkBase::UnknownTypeSignalValue> signalValue = std::make_unique<PySysLinkBase::SignalValue<double>>(this->value);
        auto outputPort = std::make_unique<PySysLinkBase::OutputPort>(PySysLinkBase::OutputPort(std::move(signalValue)));
        this->outputPorts.push_back(std::move(outputPort));
    }

    const std::vector<PySysLinkBase::SampleTime>& Constant::GetSampleTimes() const
    {
        return this->sampleTimes;
    }

    const std::vector<std::unique_ptr<PySysLinkBase::OutputPort>>& Constant::ComputeOutputsOfBlock(PySysLinkBase::SampleTime sampleTime)
    {

        std::unique_ptr<PySysLinkBase::UnknownTypeSignalValue> outputValue = this->outputPorts[0]->GetValue();
        std::unique_ptr<PySysLinkBase::SignalValue<double>> outputValueDouble = outputValue->TryCastToTyped<double>();
        outputValueDouble->SetPayload(this->value);
        this->outputPorts[0]->SetValue(std::make_unique<PySysLinkBase::SignalValue<double>>(*outputValueDouble));

        // this->outputPorts[0]->SetValue(std::make_unique<PySysLinkBase::SignalValue<double>>(outputValueDouble));
        return this->GetOutputPorts();
    }   
} // namespace BlockLibraries::BasicBlocks

