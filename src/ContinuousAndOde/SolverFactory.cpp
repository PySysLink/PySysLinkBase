#include "SolverFactory.h"
#include "OdeintStepSolver.h"
#include "EulerForwardStepSolver.h"
#include "spdlog/spdlog.h"

namespace PySysLinkBase
{
    std::shared_ptr<IOdeStepSolver> SolverFactory::CreateOdeStepSolver(std::map<std::string, ConfigurationValue> solverConfiguration)
    {
        std::string solverType = ConfigurationValueManager::TryGetConfigurationValue<std::string>("Type", solverConfiguration);

        if (solverType == "odeint")
        {
            std::string controlledSolver = ConfigurationValueManager::TryGetConfigurationValue<std::string>("ControlledSolver", solverConfiguration);

            double absoluteTolerance = 1e-8;
            double relativeTolerance = 1e-8;
            try
            {
                absoluteTolerance = ConfigurationValueManager::TryGetConfigurationValue<double>("AbsoluteTolerance", solverConfiguration);
            }
            catch (std::out_of_range const& ex)
            {
                spdlog::get("default_pysyslink")->debug("Absolute tolerance not found in configuration, using default value: {}", absoluteTolerance);
            }
            try
            {
                relativeTolerance = ConfigurationValueManager::TryGetConfigurationValue<double>("RelativeTolerance", solverConfiguration);
            }
            catch (std::out_of_range const& ex)
            {
                spdlog::get("default_pysyslink")->debug("Relative tolerance not found in configuration, using default value: {}", relativeTolerance);
            }

            if (controlledSolver == "runge_kutta_cash_karp54")
            {
                auto controlledStepper = boost::numeric::odeint::make_controlled(absoluteTolerance, relativeTolerance, boost::numeric::odeint::runge_kutta_cash_karp54<std::vector<double>>());
                using controlledStepperType = decltype(boost::numeric::odeint::make_controlled(absoluteTolerance, relativeTolerance, boost::numeric::odeint::runge_kutta_cash_karp54<std::vector<double>>()));
                return std::make_shared<OdeintStepSolver<controlledStepperType>>(controlledStepper);
            }
            else if (controlledSolver == "runge_kutta_dopri5")
            {
                auto controlledStepper = boost::numeric::odeint::make_controlled(absoluteTolerance, relativeTolerance, boost::numeric::odeint::runge_kutta_dopri5<std::vector<double>>());
                using controlledStepperType = decltype(boost::numeric::odeint::make_controlled(absoluteTolerance, relativeTolerance, boost::numeric::odeint::runge_kutta_dopri5<std::vector<double>>()));
                return std::make_shared<OdeintStepSolver<controlledStepperType>>(controlledStepper);
            }
            else if (controlledSolver == "runge_kutta_fehlberg78")
            {
                auto controlledStepper = boost::numeric::odeint::make_controlled(absoluteTolerance, relativeTolerance, boost::numeric::odeint::runge_kutta_fehlberg78<std::vector<double>>());
                using controlledStepperType = decltype(boost::numeric::odeint::make_controlled(absoluteTolerance, relativeTolerance, boost::numeric::odeint::runge_kutta_fehlberg78<std::vector<double>>()));
                return std::make_shared<OdeintStepSolver<controlledStepperType>>(controlledStepper);
            }
            // else if (controlledSolver == "rosenbrock4_controller") // TODO: this does not seem to work
            // {
            //     using rosenbrock_stepper = boost::numeric::odeint::rosenbrock4<std::vector<double>>;
            //     return std::make_shared<OdeintStepSolver<boost::numeric::odeint::rosenbrock4_controller<rosenbrock_stepper>>>();
            // }
            // else if (controlledSolver == "bulirsch_stoer")
            // {
            //     auto controlledStepper = boost::numeric::odeint::make_controlled(absoluteTolerance, relativeTolerance, boost::numeric::odeint::bulirsch_stoer<std::vector<double>>());
            //     using controlledStepperType = decltype(boost::numeric::odeint::make_controlled(absoluteTolerance, relativeTolerance, boost::numeric::odeint::bulirsch_stoer<std::vector<double>>()));
            //     return std::make_shared<OdeintStepSolver<controlledStepperType>>(controlledStepper);
            // }
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
