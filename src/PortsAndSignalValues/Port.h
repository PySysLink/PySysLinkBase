#ifndef PY_SYS_LINK_BASE_PORTS_AND_SIGNAL_VALUES_PORT
#define PY_SYS_LINK_BASE_PORTS_AND_SIGNAL_VALUES_PORT

#include <string>
#include "UnknownTypeSignalValue.h"
#include <memory>

namespace PySysLinkBase
{
    class Port {
    private:
        std::unique_ptr<UnknownTypeSignalValue> value;
    public:
        Port(std::unique_ptr<UnknownTypeSignalValue> value);

        void TryCopyValueToPort(Port& otherPort) const;

        void SetValue(std::unique_ptr<UnknownTypeSignalValue> value);
        std::unique_ptr<UnknownTypeSignalValue> GetValue() const;
    };
}

#endif /* PY_SYS_LINK_BASE_PORTS_AND_SIGNAL_VALUES_PORT */
