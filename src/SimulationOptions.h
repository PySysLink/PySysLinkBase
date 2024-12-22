#ifndef SRC_SIMULATION_OPTIONS
#define SRC_SIMULATION_OPTIONS

namespace PySysLinkBase
{
    class SimulationOptions
    {
        public:
        SimulationOptions() = default;

        double startTime;
        double stopTime;

        bool runInNaturalTime = false;
    };
} // namespace PySysLinkBase


#endif /* SRC_SIMULATION_OPTIONS */
