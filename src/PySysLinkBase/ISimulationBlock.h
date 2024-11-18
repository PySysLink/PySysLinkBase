#ifndef SRC_PY_SYS_LINK_BASE_ISIMULATION_BLOCK
#define SRC_PY_SYS_LINK_BASE_ISIMULATION_BLOCK

#include <string>
#include <vector>
#include <memory>
#include "PortsAndSignalValues/InputPort.h"
#include "PortsAndSignalValues/OutputPort.h"
#include "SampleTime.h"
#include "ContinuousState.h"

namespace PySysLinkBase
{
    class ISimulationBlock {

    public:
        virtual ~ISimulationBlock() = default;

        virtual const std::vector<SampleTime>& GetSampleTimes() const = 0;

        virtual std::vector<std::unique_ptr<PySysLinkBase::InputPort>>& GetInputPorts() = 0;
        virtual const std::vector<std::unique_ptr<PySysLinkBase::OutputPort>>& GetOutputPorts() const = 0;

        virtual const std::vector<std::unique_ptr<PySysLinkBase::OutputPort>>& ComputeOutputsOfBlock(PySysLinkBase::SampleTime sampleTime) = 0;
    };
}

#endif /* SRC_PY_SYS_LINK_BASE_ISIMULATION_BLOCK */
