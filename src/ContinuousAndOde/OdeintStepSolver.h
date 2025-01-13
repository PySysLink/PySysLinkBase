#ifndef SRC_CONTINUOUS_AND_ODE_ODEINT_STEP_SOLVER
#define SRC_CONTINUOUS_AND_ODE_ODEINT_STEP_SOLVER


#include <tuple>
#include <vector>
#include <functional>
#include "IOdeStepSolver.h"

namespace PySysLinkBase
{
    class OdeintStepSolver : public IOdeStepSolver
    {
        public:
            std::tuple<bool, std::vector<double>, double> SolveStep(std::function<std::vector<double>(std::vector<double>, double)> system, 
                                                                    std::vector<double> states_0, double currentTime, double timeStep);
    };
} // namespace PySysLinkBase

#endif /* SRC_CONTINUOUS_AND_ODE_ODEINT_STEP_SOLVER */
