#include "OdeintStepSolver.h"
#include <boost/numeric/odeint.hpp>
#include <spdlog/spdlog.h>

namespace PySysLinkBase
{
    using namespace boost::numeric::odeint;

    // Typedefs for convenience
    using State = std::vector<double>;
    using Stepper = controlled_runge_kutta<runge_kutta_cash_karp54<State>>;

    std::tuple<bool, std::vector<double>, double> OdeintStepSolver::SolveStep(
        std::function<std::vector<double>(std::vector<double>, double)> system, 
        std::vector<double> states_0, 
        double currentTime, 
        double timeStep)
    {
        // Define the system function in the format expected by ODEINT
        auto systemFunction = [&system](const State &x, State &dxdt, double t) {
            std::vector<double> gradient = system(x, t);
            dxdt = gradient; // Assign the computed derivative
        };

        // Create the stepper
        Stepper stepper;

        // Integrate a single step
        State newStates = states_0; // Initial state
        double dt = timeStep;

        controlled_step_result result = stepper.try_step(systemFunction, newStates, currentTime, dt);

        // Debug log output
        if (result == success)
        {
            return {true, newStates, dt};
        }
        else
        {
            return {false, newStates, dt};
        }
    }
} // namespace PySysLinkBase
