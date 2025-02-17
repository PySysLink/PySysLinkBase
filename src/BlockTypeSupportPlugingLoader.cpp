#include "BlockTypeSupportPlugingLoader.h"
#include <filesystem>
#include "spdlog/spdlog.h"

namespace PySysLinkBase
{
    std::map<std::string, std::shared_ptr<IBlockFactory>> BlockTypeSupportPlugingLoader::LoadPlugins(const std::string& pluginDirectory) {
        std::map<std::string, std::shared_ptr<IBlockFactory>> factoryRegistry;
        
        for (const auto& pluginPath : this->FindSharedLibraries(pluginDirectory)) {
            void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY);
            if (!handle) {
                spdlog::get("default_pysyslink")->error("Failed to load plugin: {}", pluginPath);
                spdlog::get("default_pysyslink")->error(dlerror());
                continue;
            }

            auto registerFuncFactory = reinterpret_cast<void(*)(std::map<std::string, std::shared_ptr<IBlockFactory>>&)>(dlsym(handle, "RegisterBlockFactories"));

            if (!registerFuncFactory) {
                spdlog::get("default_pysyslink")->error("Failed to find RegisterBlockFactories entry point in: ", pluginPath);
                spdlog::get("default_pysyslink")->error(dlerror());
                dlclose(handle);
                continue;
            }
            registerFuncFactory(factoryRegistry);

            auto registerFuncLogger = reinterpret_cast<void(*)(std::shared_ptr<spdlog::logger>)>(dlsym(handle, "RegisterSpdlogLogger"));

            if (!registerFuncLogger) {
                spdlog::get("default_pysyslink")->error("Failed to find RegisterSpdlogLogger entry point in: ", pluginPath);
                spdlog::get("default_pysyslink")->error(dlerror());
                dlclose(handle);
                continue;
            }
            registerFuncLogger(spdlog::get("default_pysyslink"));
            
            spdlog::get("default_pysyslink")->debug("Pluging loaded: {}", pluginPath);
        }
        return factoryRegistry;
    }

    std::vector<std::string> BlockTypeSupportPlugingLoader::FindSharedLibraries(const std::string& searchDirectory) {
        std::vector<std::string> sharedLibraries;

        try {
            // Iterate over the contents of the search directory
            for (const auto& entry : std::filesystem::directory_iterator(searchDirectory)) {
                if (entry.is_regular_file()) {
                    // Get the filename
                    const std::string filename = entry.path().filename().string();

                    // Check if the filename matches the desired pattern
                    if (filename.find("libBlockTypeSupports") == 0 && this->StringEndsWith(filename, ".so")) {
                        sharedLibraries.push_back(entry.path().string());
                    }
                }
            }
        } catch (const std::filesystem::filesystem_error& e) {
            spdlog::get("default_pysyslink")->error("Error accessing directory: ", e.what());
        }

        return sharedLibraries;
    }

    bool BlockTypeSupportPlugingLoader::StringEndsWith(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() &&
            str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
} // namespace PySysLinkBase
