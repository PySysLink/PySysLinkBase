#include "Display.h"
#include <PySysLinkBase/PortsAndSignalValues/SignalValue.h>
#include <PySysLinkBase/PortsAndSignalValues/UnknownTypeSignalValue.h>
#include <vector>
#include <iostream>

namespace BlockLibraries::BasicBlocks
{
    Display::Display()
    {
        this->sampleTimes.push_back(PySysLinkBase::SampleTime(PySysLinkBase::SampleTimeType::inherited, 
                                                            std::vector<PySysLinkBase::SampleTimeType>{PySysLinkBase::SampleTimeType::constant,
                                                                                                       PySysLinkBase::SampleTimeType::continuous,
                                                                                                       PySysLinkBase::SampleTimeType::discrete}));
    
        std::unique_ptr<PySysLinkBase::UnknownTypeSignalValue> signalValue = std::make_unique<PySysLinkBase::SignalValue<double>>(PySysLinkBase::SignalValue<double>(0.0));
        auto inputPort = std::make_unique<PySysLinkBase::InputPort>(PySysLinkBase::InputPort(false, std::move(signalValue)));
        this->inputPorts.push_back(std::move(inputPort));
    }

    const std::vector<PySysLinkBase::SampleTime>& Display::GetSampleTimes() const
    {
        return this->sampleTimes;
    }

    const std::vector<std::unique_ptr<PySysLinkBase::OutputPort>>& Display::ComputeOutputsOfBlock(PySysLinkBase::SampleTime sampleTime)
    {
        std::unique_ptr<PySysLinkBase::UnknownTypeSignalValue> inputValue = this->inputPorts[0]->GetValue();
        auto inputValueDouble = inputValue->TryCastToTyped<double>();
        std::cout << "Display value: " << inputValueDouble->GetPayload() << std::endl;

        return this->GetOutputPorts();
    }   
} // namespace BlockLibraries::BasicBlocks

