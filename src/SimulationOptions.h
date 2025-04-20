#ifndef SRC_SIMULATION_OPTIONS
#define SRC_SIMULATION_OPTIONS

#include <vector>
#include <utility>
#include <string>
#include <tuple>
#include "ConfigurationValue.h"

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

        std::vector<std::tuple<std::string, std::string, int>> blockIdsInputOrOutputAndIndexesToLog = {};

        std::map<std::string, std::map<std::string, ConfigurationValue>> solversConfiguration;

        std::string hdf5FileName = "";
        bool saveToFileContinuously = false;

        bool saveToVectors = true;
    };
} // namespace PySysLinkBase


#endif /* SRC_SIMULATION_OPTIONS */
