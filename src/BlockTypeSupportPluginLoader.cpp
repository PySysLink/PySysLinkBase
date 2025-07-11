#include "BlockTypeSupportPluginLoader.h"
#include <filesystem>
#include "spdlog/spdlog.h"

namespace PySysLinkBase
{
    std::map<std::string, std::shared_ptr<IBlockFactory>> BlockTypeSupportPluginLoader::LoadPlugins(const std::string& pluginDirectory, std::map<std::string, PySysLinkBase::ConfigurationValue> pluginConfiguration) {
        std::map<std::string, std::shared_ptr<IBlockFactory>> factoryRegistry;
        
        for (const auto& pluginPath : this->FindSharedLibraries(pluginDirectory)) {
            spdlog::get("default_pysyslink")->debug("Trying to open plugin: {}", pluginPath);

            void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY);
            if (!handle) {
                spdlog::get("default_pysyslink")->error("Failed to load plugin: {}", pluginPath);
                spdlog::get("default_pysyslink")->error(dlerror());
                continue;
            }

            auto registerFuncLogger = reinterpret_cast<void(*)(std::shared_ptr<spdlog::logger>)>(dlsym(handle, "RegisterSpdlogLogger"));

            if (!registerFuncLogger) {
                spdlog::get("default_pysyslink")->error("Failed to find RegisterSpdlogLogger entry point in: ", pluginPath);
                spdlog::get("default_pysyslink")->error(dlerror());
                continue;
            }

            spdlog::get("default_pysyslink")->debug("registerFuncLogger opened on plugin: {}", pluginPath);

            registerFuncLogger(spdlog::get("default_pysyslink"));
            
            spdlog::get("default_pysyslink")->debug("registerFuncLogger called on plugin: {}", pluginPath);

            auto registerFuncFactory = reinterpret_cast<void(*)(std::map<std::string, std::shared_ptr<IBlockFactory>>&, std::map<std::string, PySysLinkBase::ConfigurationValue>)>(dlsym(handle, "RegisterBlockFactories"));

            if (!registerFuncFactory) {
                spdlog::get("default_pysyslink")->error("Failed to find RegisterBlockFactories entry point in: ", pluginPath);
                spdlog::get("default_pysyslink")->error(dlerror());
                continue;
            }

            spdlog::get("default_pysyslink")->debug("registerFuncFactory opened on plugin: {}", pluginPath);

            registerFuncFactory(factoryRegistry, pluginConfiguration);

            spdlog::get("default_pysyslink")->debug("registerFuncFactory called on plugin: {}", pluginPath);

            spdlog::get("default_pysyslink")->debug("Plugin loaded: {}", pluginPath);
            spdlog::get("default_pysyslink")->debug("Dll closed");
        }
        return factoryRegistry;
    }

    std::vector<std::string> BlockTypeSupportPluginLoader::FindSharedLibraries(const std::string& searchDirectory) {
        std::vector<std::string> sharedLibraries;

        try {
            // Use recursive_directory_iterator to look in subfolders (depth 1)
            for (auto it = std::filesystem::recursive_directory_iterator(searchDirectory); it != std::filesystem::recursive_directory_iterator(); ++it) {
                if (it.depth() > 1) continue; // Only top-level and first-level subfolders
                if (it->is_regular_file()) {
                    const std::string filename = it->path().filename().string();
                    if (filename.find("libBlockTypeSupports") == 0 && this->StringEndsWith(filename, ".so")) {
                        sharedLibraries.push_back(it->path().string());
                    }
                }
            }
        } catch (const std::filesystem::filesystem_error& e) {
            spdlog::get("default_pysyslink")->error("Error accessing directory: ", e.what());
        }

        return sharedLibraries;
    }

    bool BlockTypeSupportPluginLoader::StringEndsWith(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() &&
            str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
} // namespace PySysLinkBase
