#include "BlockTypeSupportPlugingLoader.h"
#include <filesystem>
#include <iostream>

namespace PySysLinkBase
{
    std::map<std::string, std::unique_ptr<IBlockFactory>> BlockTypeSupportPlugingLoader::LoadPlugins(const std::string& pluginDirectory) {
        std::map<std::string, std::unique_ptr<IBlockFactory>> factoryRegistry;
        
        for (const auto& pluginPath : this->FindSharedLibraries(pluginDirectory)) {
            void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY);
            if (!handle) {
                std::cerr << "Failed to load plugin: " << pluginPath << std::endl;
                std::cerr << dlerror() << std::endl;
                continue;
            }

            auto registerFunc = reinterpret_cast<void(*)(std::map<std::string, std::unique_ptr<IBlockFactory>>&)>(dlsym(handle, "RegisterBlockFactories"));

            if (!registerFunc) {
                std::cerr << "Failed to find entry point in: " << pluginPath << std::endl;
                std::cerr << dlerror() << std::endl;
                dlclose(handle);
                continue;
            }
            std::cout << "Pluging loaded: " << pluginPath << std::endl;
            registerFunc(factoryRegistry);
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
            std::cerr << "Error accessing directory: " << e.what() << "\n";
        }

        return sharedLibraries;
    }

    bool BlockTypeSupportPlugingLoader::StringEndsWith(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() &&
            str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
} // namespace PySysLinkBase
