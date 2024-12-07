#ifndef SRC_PORTS_AND_SIGNAL_VALUES_PORT
#define SRC_PORTS_AND_SIGNAL_VALUES_PORT

#include <string>
#include "UnknownTypeSignalValue.h"
#include <memory>



namespace PySysLinkBase
{
    class ISimulationBlock;

    class Port {
    protected:
        std::unique_ptr<UnknownTypeSignalValue> value;
        ISimulationBlock& ownerBlock;
    public:
        Port(std::unique_ptr<UnknownTypeSignalValue> value, ISimulationBlock& ownerBlock);

        void TryCopyValueToPort(Port& otherPort) const;

        ISimulationBlock& GetOwnerBlock() const;

        void SetValue(std::unique_ptr<UnknownTypeSignalValue> value);
        std::unique_ptr<UnknownTypeSignalValue> GetValue() const;
    };
}

#endif /* SRC_PORTS_AND_SIGNAL_VALUES_PORT */
