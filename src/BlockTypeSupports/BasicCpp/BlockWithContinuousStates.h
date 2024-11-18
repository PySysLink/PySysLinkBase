#ifndef BLOCK_TYPE_SUPPORTS_BASIC_CPP_BLOCK_WITH_CONTINUOUS_STATES
#define BLOCK_TYPE_SUPPORTS_BASIC_CPP_BLOCK_WITH_CONTINUOUS_STATES

#include <PySysLinkBase/ISimulationBlockWithContinuousStates.h>

namespace BlockTypeSupports::BasicCpp
{
    class SimulationBlockWithContinuousStates : public PySysLinkBase::ISimulationBlockWithContinuousStates
    {
        private:
            std::vector<PySysLinkBase::ContinuousState> ContinuousStates;
        public:
            const std::vector<PySysLinkBase::ContinuousState>& GetContinuousStates() const;
            void SetContinuousStates(std::vector<PySysLinkBase::ContinuousState> newStates);

            virtual const std::vector<double>& GetContinousStateDerivatives() const;
    };
}

#endif /* BLOCK_TYPE_SUPPORTS_BASIC_CPP_BLOCK_WITH_CONTINUOUS_STATES */
