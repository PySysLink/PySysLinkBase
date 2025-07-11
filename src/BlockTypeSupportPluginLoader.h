#ifndef SRC_PY_SYS_LINK_BASE_BLOCK_TYPE_SUPPORT_PLUGING_LOADER
#define SRC_PY_SYS_LINK_BASE_BLOCK_TYPE_SUPPORT_PLUGING_LOADER

#include <map>
#include <memory>
#include <string>
#include <dlfcn.h> // For Linux/macOS dynamic linking. Use `windows.h` for Windows.

#include "IBlockFactory.h"

namespace PySysLinkBase {

class BlockTypeSupportPluginLoader {
public:
    std::map<std::string, std::shared_ptr<IBlockFactory>> LoadPlugins(const std::string& pluginDirectory, std::map<std::string, PySysLinkBase::ConfigurationValue> pluginConfiguration);

private:
    std::vector<std::string> FindSharedLibraries(const std::string& pluginDirectory);
    bool StringEndsWith(const std::string& str, const std::string& suffix);
};

} // namespace PySysLinkBase

#endif /* SRC_PY_SYS_LINK_BASE_BLOCK_TYPE_SUPPORT_PLUGING_LOADER */
