#ifndef SRC_SPDLOG_MANAGER
#define SRC_SPDLOG_MANAGER

namespace PySysLinkBase
{
    enum LogLevel
    {
        off,
        debug,
        info,
        warning,
        error,
        critical
    };

    class SpdlogManager
    {
        public:
        static void ConfigureDefaultLogger();
        static void SetLogLevel(LogLevel logLevel);
    };
} // namespace PySysLinkBase


#endif /* SRC_SPDLOG_MANAGER */
