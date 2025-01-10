#ifndef SRC_SIMULATION_OPTIONS
#define SRC_SIMULATION_OPTIONS

#include <vector>
#include <utility>
#include <string>

namespace PySysLinkBase
{
    class SimulationOptions
    {
        public:
        SimulationOptions() = default;

        double startTime;
        double stopTime;

        bool runInNaturalTime = false;
        double naturalTimeSpeedMultiplier = 1.0;

        std::vector<std::pair<std::string, int>> blockIdsAndOutputIndexesToLog = {};
    };
} // namespace PySysLinkBase


#endif /* SRC_SIMULATION_OPTIONS */
