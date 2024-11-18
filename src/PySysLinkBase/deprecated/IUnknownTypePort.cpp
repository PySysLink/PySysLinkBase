#include "IUnknownTypePort.h"

namespace PySysLinkBase
{
    const SampleTime& IUnknownTypePort::GetSampleTime() const
    {
        return this->sampleTime;
    }
} // namespace PySysLinkBase
