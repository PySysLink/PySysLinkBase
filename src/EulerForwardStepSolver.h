#ifndef SRC_EULER_FORWARD_STEP_SOLVER
#define SRC_EULER_FORWARD_STEP_SOLVER


#include <tuple>
#include <vector>
#include <functional>
#include "IOdeStepSolver.h"

namespace PySysLinkBase
{
    class EulerForwardStepSolver : public IOdeStepSolver
    {
        public:
            std::tuple<std::vector<double>, double> SolveStep(std::function<std::vector<double>(std::vector<double>, double)> system, 
                                                                    std::vector<double> states_0, double currentTime, double timeStep);
    };
} // namespace PySysLinkBase

#endif /* SRC_EULER_FORWARD_STEP_SOLVER */