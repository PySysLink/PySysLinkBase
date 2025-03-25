#ifndef SRC_CONTINUOUS_AND_ODE_ODEINT_STEP_SOLVER
#define SRC_CONTINUOUS_AND_ODE_ODEINT_STEP_SOLVER


#include <tuple>
#include <vector>
#include <functional>
#include "IOdeStepSolver.h"
#include <boost/numeric/odeint.hpp>

namespace PySysLinkBase
{
    template <typename T> 
    class OdeintStepSolver : public IOdeStepSolver
    {
        private:
            T controlledStepper;
        public:
            OdeintStepSolver(T controlledStepper)
            {
                this->controlledStepper = controlledStepper;
            }
            std::tuple<bool, std::vector<double>, double> SolveStep(std::function<std::vector<double>(std::vector<double>, double)> system, 
                                                                    std::vector<double> states_0, double currentTime, double timeStep)
            {
                // Define the system function in the format expected by ODEINT
                auto systemFunction = [&system](const std::vector<double> &x, std::vector<double> &dxdt, double t) {
                    std::vector<double> gradient = system(x, t);
                    dxdt = gradient; // Assign the computed derivative
                };
                
                
                // Create the stepper
                // Stepper stepper;

                // Integrate a single step
                std::vector<double> newStates = states_0; // Initial state
                double dt = timeStep;

                boost::numeric::odeint::controlled_step_result result = this->controlledStepper.try_step(systemFunction, newStates, currentTime, dt);
                // controlled_step_result result = stepper.try_step(systemFunction, newStates, currentTime, dt);

                system(states_0, currentTime); // Set initial states again, may be optimized

                // Debug log output
                if (result == boost::numeric::odeint::success)
                {
                    return {true, newStates, dt};
                }
                else
                {
                    return {false, newStates, dt};
                }
            }
    };
} // namespace PySysLinkBase

#endif /* SRC_CONTINUOUS_AND_ODE_ODEINT_STEP_SOLVER */
