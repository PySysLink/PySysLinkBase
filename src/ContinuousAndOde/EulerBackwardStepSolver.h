#ifndef SRC_EULER_BACKWARD_STEP_SOLVER
#define SRC_EULER_BACKWARD_STEP_SOLVER


#include <tuple>
#include <vector>
#include <functional>
#include <sstream>
#include "IOdeStepSolver.h"
#include <cmath>
#include <stdexcept>
#include <iostream>

namespace PySysLinkBase
{
    class EulerBackwardStepSolver : public IOdeStepSolver
    {
        public:
            EulerBackwardStepSolver(double maximumIterations = 50, double tolerance = 1e-6) 
                : maximumIterations(maximumIterations), tolerance(tolerance)
            {
            };
            virtual std::tuple<bool, std::vector<double>, double> SolveStep(std::function<std::vector<double>(std::vector<double>, double)> system, 
                                                                    std::vector<double> states_0, double currentTime, double timeStep)
            {
                throw std::runtime_error("Jacobian needed for implicit Euler method");
            }
            virtual std::tuple<bool, std::vector<double>, double> SolveStep(std::function<std::vector<double>(std::vector<double>, double)> systemDerivatives,
                                                                    std::function<std::vector<std::vector<double>>(std::vector<double>, double)> systemJacobian, 
                                                                    std::vector<double> states_0, double currentTime, double timeStep);
            virtual bool IsJacobianNeeded() const 
            {
                return true;
            }          
        
        private:
            double maximumIterations;
            double tolerance;
            std::vector<double> ComputeNewtonStep(const std::vector<std::vector<double>>& systemJacobianEnd,
                const std::vector<double>& systemDerivativesEnd, const std::vector<double>& states_0, const std::vector<double>& statesEnd,
                 double timeStep);
    };
} // namespace PySysLinkBase

#endif /* SRC_EULER_FORWARD_STEP_SOLVER */
