#ifndef SRC_PORTS_AND_SIGNAL_VALUES_PORT
#define SRC_PORTS_AND_SIGNAL_VALUES_PORT

#include <string>
#include "UnknownTypeSignalValue.h"
#include <memory>
#include <functional>


namespace PySysLinkBase
{
    class ISimulationBlock;

    class Port {
    protected:
        std::shared_ptr<UnknownTypeSignalValue> value;
        
    public:
        Port(std::shared_ptr<UnknownTypeSignalValue> value);

        void TryCopyValueToPort(Port& otherPort) const;

        void SetValue(std::shared_ptr<UnknownTypeSignalValue> value);
        std::shared_ptr<UnknownTypeSignalValue> GetValue() const;

        bool operator==(const Port& rhs) const
        {
            return this == &rhs;
        }
    };
}

#endif /* SRC_PORTS_AND_SIGNAL_VALUES_PORT */
