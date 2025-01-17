#ifndef SRC_CONTINUOUS_AND_ODE_SOLVER_FACTORY
#define SRC_CONTINUOUS_AND_ODE_SOLVER_FACTORY

#include <memory>
#include "IOdeStepSolver.h"
#include <map>
#include "../ConfigurationValue.h"

namespace PySysLinkBase
{
    class SolverFactory
    {
        public:
            static std::shared_ptr<IOdeStepSolver> CreateOdeStepSolver(std::map<std::string, ConfigurationValue> solverConfiguration);
    };
} // namespace PySysLinkBase


#endif /* SRC_CONTINUOUS_AND_ODE_SOLVER_FACTORY */
