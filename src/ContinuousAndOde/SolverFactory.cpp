#include "SolverFactory.h"
#include "OdeintStepSolver.h"
#include "EulerForwardStepSolver.h"

namespace PySysLinkBase
{
    std::shared_ptr<IOdeStepSolver> SolverFactory::CreateOdeStepSolver(std::map<std::string, ConfigurationValue> solverConfiguration)
    {
        std::string solverType = ConfigurationValueManager::TryGetConfigurationValue<std::string>("Type", solverConfiguration);

        if (solverType == "odeint")
        {
            std::string controlledSolver = ConfigurationValueManager::TryGetConfigurationValue<std::string>("ControlledSolver", solverConfiguration);
            if (controlledSolver == "runge_kutta_cash_karp54")
            {
                return std::make_shared<OdeintStepSolver<boost::numeric::odeint::controlled_runge_kutta<boost::numeric::odeint::runge_kutta_cash_karp54<std::vector<double>>>>>();
            }
            else if (controlledSolver == "runge_kutta_dopri5")
            {
                return std::make_shared<OdeintStepSolver<boost::numeric::odeint::controlled_runge_kutta<boost::numeric::odeint::runge_kutta_dopri5<std::vector<double>>>>>();
            }
            else if (controlledSolver == "runge_kutta_fehlberg78")
            {
                return std::make_shared<OdeintStepSolver<boost::numeric::odeint::controlled_runge_kutta<boost::numeric::odeint::runge_kutta_fehlberg78<std::vector<double>>>>>();
            }
            // else if (controlledSolver == "rosenbrock4_controller") // TODO: this does not seem to work
            // {
            //     using rosenbrock_stepper = boost::numeric::odeint::rosenbrock4<std::vector<double>>;
            //     return std::make_shared<OdeintStepSolver<boost::numeric::odeint::rosenbrock4_controller<rosenbrock_stepper>>>();
            // }
            else if (controlledSolver == "bulirsch_stoer")
            {
                return std::make_shared<OdeintStepSolver<boost::numeric::odeint::bulirsch_stoer<std::vector<double>>>>();
            }
            else
            {
                throw std::invalid_argument("Controlled solver not recognized");
            }
        }
        else if (solverType == "EulerForward")
        {
            return std::make_shared<EulerForwardStepSolver>();
        }
        else
        {
            throw std::invalid_argument("Solver type not recognized");
        }
    }
} // namespace PySysLinkBase
