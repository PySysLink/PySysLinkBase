#ifndef SRC_CONTINUOUS_AND_ODE_IODE_STEP_SOLVER
#define SRC_CONTINUOUS_AND_ODE_IODE_STEP_SOLVER

#include <tuple>
#include <vector>
#include <functional>

namespace PySysLinkBase
{
    class IOdeStepSolver
    {
        public:
            virtual std::tuple<bool, std::vector<double>, double> SolveStep(std::function<std::vector<double>(std::vector<double>, double)> system, 
                                                                    std::vector<double> states_0, double currentTime, double timeStep) = 0;
    };
} // namespace PySysLinkBase


#endif /* SRC_CONTINUOUS_AND_ODE_IODE_STEP_SOLVER */
