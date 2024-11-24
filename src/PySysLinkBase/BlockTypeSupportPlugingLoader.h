#ifndef SRC_PY_SYS_LINK_BASE_BLOCK_TYPE_SUPPORT_PLUGING_LOADER
#define SRC_PY_SYS_LINK_BASE_BLOCK_TYPE_SUPPORT_PLUGING_LOADER

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <dlfcn.h> // For Linux/macOS dynamic linking. Use `windows.h` for Windows.

#include "IBlockFactory.h"

namespace PySysLinkBase {

class PluginLoader {
public:
    void LoadPlugins(const std::string& pluginDirectory) {
        // Example: Iterate through files in the pluginDirectory (you'll need to implement this)
        for (const auto& pluginPath : FindSharedLibraries(pluginDirectory)) {
            void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY);
            if (!handle) {
                std::cerr << "Failed to load plugin: " << pluginPath << "\n";
                std::cerr << dlerror() << "\n";
                continue;
            }

            // Get the entry point function
            auto registerFunc = reinterpret_cast<void(*)(std::map<std::string, std::unique_ptr<IBlockFactory>>&)>(dlsym(handle, "RegisterBlockFactories"));

            if (!registerFunc) {
                std::cerr << "Failed to find entry point in: " << pluginPath << "\n";
                std::cerr << dlerror() << "\n";
                dlclose(handle);
                continue;
            }

            // Call the function to register block factories
            registerFunc(factoryRegistry);
        }
    }

    std::unique_ptr<IBlockFactory> GetFactory(const std::string& factoryName) {
        auto it = factoryRegistry.find(factoryName);
        if (it != factoryRegistry.end()) {
            return std::move(it->second);
        }
        throw std::runtime_error("Factory not found: " + factoryName);
    }

private:
    std::map<std::string, std::unique_ptr<IBlockFactory>> factoryRegistry;

    // Example stub for finding shared libraries in a directory
    std::vector<std::string> FindSharedLibraries(const std::string& pluginDirectory) {
        // Use directory traversal to return paths of `.so`, `.dll`, or `.dylib` files
        return {"path/to/plugin1.so", "path/to/plugin2.so"}; // Replace with real implementation
    }
};

} // namespace PySysLinkBase

#endif /* SRC_PY_SYS_LINK_BASE_BLOCK_TYPE_SUPPORT_PLUGING_LOADER */
