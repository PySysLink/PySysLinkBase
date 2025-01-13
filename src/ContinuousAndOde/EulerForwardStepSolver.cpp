#include "EulerForwardStepSolver.h"
#include <spdlog/spdlog.h>

namespace PySysLinkBase
{
    std::tuple<bool, std::vector<double>, double> EulerForwardStepSolver::SolveStep(std::function<std::vector<double>(std::vector<double>, double)> system, std::vector<double> states_0, double currentTime, double timeStep)
    {
        std::vector<double> gradient = system(states_0, currentTime);
        spdlog::get("default_pysyslink")->debug("Gradient size: {}", gradient.size());
        for (const auto& value: gradient)
        {
            spdlog::get("default_pysyslink")->debug(value);
        }

        std::vector<double> newStates(gradient.size(), 0.0);
        for (int i = 0; i < gradient.size(); i++)
        {
            newStates[i] = states_0[i] + gradient[i] * timeStep;
        }
        return {true, newStates, timeStep};
    }

} // namespace PySysLinkBase
