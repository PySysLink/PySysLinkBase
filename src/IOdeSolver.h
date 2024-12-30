#ifndef SRC_IODE_SOLVER
#define SRC_IODE_SOLVER

#include "ISimulationBlockWithContinuousStates.h"
#include <memory>

namespace PySysLinkBase
{
    class IOdeSolver
    {
        public:
            virtual double DoStepForBlocks(std::vector<std::shared_ptr<ISimulationBlockWithContinuousStates>> blocks, double currentTime) = 0;
    };
} // namespace PySysLinkBase


#endif /* SRC_IODE_SOLVER */
