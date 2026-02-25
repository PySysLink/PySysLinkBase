// src/main.cpp

#include <argparse/argparse.hpp>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "PySysLinkBase/SimulationModel.h"
#include "PySysLinkBase/ModelParser.h"
#include "PySysLinkBase/IBlockFactory.h"
#include "PySysLinkBase/BlockTypeSupportPluginLoader.h"
#include "PySysLinkBase/SimulationManager.h"
#include "PySysLinkBase/SpdlogManager.h"
#include "PySysLinkBase/BlockEventsHandler.h"
#include "PySysLinkBase/SimulationOptions.h"
#include "PySysLinkBase/SimulationOutput.h"

struct SimulationOptionsYaml {
    double startTime;
    double stopTime;
    bool runInNaturalTime;
    double naturalTimeSpeedMultiplier;

    std::vector<std::string> pluginDirs;
    std::vector<std::tuple<std::string,std::string,int>> logs;

    // Optional / advanced
    std::map<std::string,
        std::map<std::string, PySysLinkBase::ConfigurationValue>> solversConfiguration;
    
    std::map<std::string, PySysLinkBase::ConfigurationValue> pluginConfiguration;

    std::string hdf5FileName = "";
    bool saveToFileContinuously = false;
    bool saveToVectors = true;

    bool saveToJson = false;
    std::string outputJsonFile;
};

struct YamlError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

inline std::string loc(const YAML::Node& n) {
    if (!n.Mark().is_null()) {
        return " (line " + std::to_string(n.Mark().line + 1) +
               ", column " + std::to_string(n.Mark().column + 1) + ")";
    }
    return "";
}

template<typename T>
T get_required(
    const YAML::Node& parent,
    const std::string& key,
    const std::string& path
) {
    if (!parent[key]) {
        throw YamlError(
            "Missing required key: " + path + "." + key + loc(parent)
        );
    }

    try {
        return parent[key].as<T>();
    } catch (const YAML::Exception& e) {
        throw YamlError(
            "Invalid value for key: " + path + "." + key +
            loc(parent[key]) + "\n  Reason: " + e.what()
        );
    }
}

// Optional keys helper
template<typename T>
T get_optional(
    const YAML::Node& parent,
    const std::string& key,
    const T& defaultValue
) {
    if (!parent[key]) return defaultValue;
    try {
        return parent[key].as<T>();
    } catch (const YAML::Exception& e) {
        throw YamlError("Invalid value for optional key: " + key);
    }
}

namespace YAML {

template<>
struct convert<SimulationOptionsYaml> {
    static bool decode(const Node& node, SimulationOptionsYaml& rhs) {
        if (!node.IsMap()) {
            throw YamlError("Root of options YAML must be a map");
        }

        const std::string path = "options";

        rhs.startTime = get_required<double>(node, "StartTime", path);
        rhs.stopTime  = get_required<double>(node, "StopTime", path);
        rhs.runInNaturalTime =
            get_required<bool>(node, "RunInNaturalTime", path);
        rhs.naturalTimeSpeedMultiplier =
            get_required<double>(node, "NaturalTimeSpeedMultiplier", path);
        rhs.pluginDirs = get_required<std::vector<std::string>>(node, "PluginDirs", path);
        
        rhs.hdf5FileName =
            get_optional<std::string>(node, "HDF5FileName", "");

        rhs.saveToFileContinuously =
            get_optional<bool>(node, "SaveToFileContinuously", false);

        rhs.saveToVectors =
            get_optional<bool>(node, "SaveToVectors", true);

        rhs.saveToJson =
            get_optional<bool>(node, "SaveToJson", false);

        rhs.outputJsonFile =
            get_optional<std::string>(node, "OutputJsonFile", "");
            
        const auto logsNode =
            get_required<YAML::Node>(node, "BlockIdsInputOrOutputAndIndexesToLog", path);

        if (!logsNode.IsSequence()) {
            throw YamlError(
                path + ".BlockIdsInputOrOutputAndIndexesToLog must be a sequence" +
                loc(logsNode)
            );
        }

        for (std::size_t i = 0; i < logsNode.size(); ++i) {
            const auto& entry = logsNode[i];
            const std::string entryPath =
                path + ".BlockIdsInputOrOutputAndIndexesToLog[" + std::to_string(i) + "]";

            if (!entry.IsSequence() || entry.size() != 3) {
                throw YamlError(
                    entryPath + " must be [string, string, int]" +
                    loc(entry)
                );
            }

            rhs.logs.emplace_back(
                entry[0].as<std::string>(),
                entry[1].as<std::string>(),
                entry[2].as<int>()
            );
        }

        if (node["SolversConfiguration"]) {
            for (const auto& outer : node["SolversConfiguration"]) {
                std::map<std::string, PySysLinkBase::ConfigurationValue> inner;
                for (const auto& inner_pair : outer.second) {
                    inner[inner_pair.first.as<std::string>()] =
                        PySysLinkBase::ModelParser::YamlToConfigurationValue(inner_pair.second);
                }
                rhs.solversConfiguration[outer.first.as<std::string>()] = inner;
            }
        }

        if (node["PluginConfiguration"]) {
            const auto& pcNode = node["PluginConfiguration"];

            if (!pcNode.IsMap()) {
                throw YamlError(
                    "options.PluginConfiguration must be a map" + loc(pcNode)
                );
            }

            for (const auto& it : pcNode) {
                const std::string key = it.first.as<std::string>();
                try {
                    rhs.pluginConfiguration[key] =
                        PySysLinkBase::ModelParser::YamlToConfigurationValue(it.second);
                } catch (const std::exception& e) {
                    throw YamlError(
                        "Invalid value in options.PluginConfiguration." + key +
                        loc(it.second) + "\n  Reason: " + e.what()
                    );
                }
            }
        }

        return true;
    }
};

}

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("PySysLinkBase");

    program.add_argument("model_yaml")
        .help("Path to the simulation model (YAML)");

    program.add_argument("options_yaml")
        .help("Path to the options file (YAML), containing the plugin paths, simulation options and output options");
    
    program.add_argument("--verbose")
        .help("increase output verbosity")
        .default_value(false)
        .implicit_value(true);

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    PySysLinkBase::SpdlogManager::ConfigureDefaultLogger();
    if (program["--verbose"] == true) 
    {
        PySysLinkBase::SpdlogManager::SetLogLevel(PySysLinkBase::LogLevel::debug);
    }
    else 
    {
        PySysLinkBase::SpdlogManager::SetLogLevel(PySysLinkBase::LogLevel::warning);
    }

    YAML::Node opts;

    try
    {
        opts = YAML::LoadFile(program.get<std::string>("options_yaml"));
    }
    catch (const YAML::BadFile &e)
    {
        std::cerr << "Could not read file: " << program.get<std::string>("options_yaml") << "\n";
        return 1;
    }

    SimulationOptionsYaml cfg;
    try {
        cfg = opts.as<SimulationOptionsYaml>();
    }
    catch (const YamlError& e) {
        std::cerr << "Options YAML error:\n" << e.what() << std::endl;
        return 1;
    }
    catch (const YAML::Exception& e) {
        std::cerr << "YAML parsing error:\n" << e.what() << std::endl;
        return 1;
    }



    // 3) Load all block‐factory plugins
    auto blockEventsHandler = std::make_shared<PySysLinkBase::BlockEventsHandler>();
    std::map<std::string,std::shared_ptr<PySysLinkBase::IBlockFactory>> blockFactories;

    const auto& pluginConfiguration = cfg.pluginConfiguration;

    for (auto &dir: cfg.pluginDirs) {
        PySysLinkBase::BlockTypeSupportPluginLoader loader;
        auto factories = loader.LoadPlugins(dir, pluginConfiguration);
        blockFactories.insert(factories.begin(), factories.end());
    }

    // 4) Parse the simulation model
    auto model = PySysLinkBase::ModelParser::ParseFromYaml(
        program.get<std::string>("model_yaml"),
        blockFactories,
        blockEventsHandler
    );
    std::vector<std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>>> blockChains = model->GetDirectBlockChains();
    std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>> orderedBlocks = model->OrderBlockChainsOntoFreeOrder(blockChains);
    model->PropagateSampleTimes();

    
    
    auto simOpts = std::make_shared<PySysLinkBase::SimulationOptions>();

    simOpts->startTime = cfg.startTime;
    simOpts->stopTime  = cfg.stopTime;
    simOpts->runInNaturalTime = cfg.runInNaturalTime;
    simOpts->naturalTimeSpeedMultiplier = cfg.naturalTimeSpeedMultiplier;

    simOpts->blockIdsInputOrOutputAndIndexesToLog = cfg.logs;
    simOpts->solversConfiguration = cfg.solversConfiguration;

    simOpts->hdf5FileName = cfg.hdf5FileName;
    simOpts->saveToFileContinuously = cfg.saveToFileContinuously;
    simOpts->saveToVectors = cfg.saveToVectors;
    

    PySysLinkBase::SimulationManager mgr(model, simOpts);
    auto output = mgr.RunSimulation();

    if (cfg.saveToJson) {
        output->WriteJson(cfg.outputJsonFile);
        std::cout << "Simulation complete, output written to "
                << cfg.outputJsonFile << "\n";
    }

    return 0;
}
