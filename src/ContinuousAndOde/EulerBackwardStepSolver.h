#ifndef SRC_EULER_BACKWARD_STEP_SOLVER
#define SRC_EULER_BACKWARD_STEP_SOLVER


#include <tuple>
#include <vector>
#include <functional>
#include "IOdeStepSolver.h"

namespace PySysLinkBase
{
    class EulerBackwardStepSolver : public IOdeStepSolver
    {
        public:
            virtual std::tuple<bool, std::vector<double>, double> SolveStep(std::function<std::vector<double>(std::vector<double>, double)> system, 
                                                                    std::vector<double> states_0, double currentTime, double timeStep)
            {
                throw std::runtime_error("Jacobian needed");
            }
            virtual std::tuple<bool, std::vector<double>, double> SolveStep(std::function<std::vector<double>(std::vector<double>, double)> systemDerivatives,
                                                                    std::function<std::vector<std::vector<double>>(std::vector<double>, double)> systemJacobian, 
                                                                    std::vector<double> states_0, double currentTime, double timeStep);
            virtual bool IsJacobianNeeded() const 
            {
                return true;
            }
    };
} // namespace PySysLinkBase

#endif /* SRC_EULER_FORWARD_STEP_SOLVER */
