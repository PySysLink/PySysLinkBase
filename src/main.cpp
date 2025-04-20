// src/cli.cpp

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

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("PySysLinkBase");

    program.add_argument("model_yaml")
        .help("Path to the simulation model (YAML)");

    program.add_argument("plugin_dirs_file")
        .help("Text file listing plugin directories, one per line");

    program.add_argument("options_yaml")
        .help("Path to the simulation options file (YAML)");

    program.add_argument("output_csv")
        .help("Path to write the simulation output (CSV)");
    
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

    // 1) Setup logging
    PySysLinkBase::SpdlogManager::ConfigureDefaultLogger();
    if (program["--verbose"] == true) 
    {
        PySysLinkBase::SpdlogManager::SetLogLevel(PySysLinkBase::LogLevel::debug);
    }
    else 
    {
        PySysLinkBase::SpdlogManager::SetLogLevel(PySysLinkBase::LogLevel::warning);
    }

    // 2) Read plugin directories
    std::ifstream dirs_in(program.get<std::string>("plugin_dirs_file"));
    if (!dirs_in) {
        std::cerr << "Cannot open plugin dirs file\n";
        return 1;
    }
    std::vector<std::string> plugin_dirs;
    for (std::string line; std::getline(dirs_in, line); )
        if (!line.empty()) plugin_dirs.push_back(line);

    // 3) Load all block‐factory plugins
    auto blockEventsHandler = std::make_shared<PySysLinkBase::BlockEventsHandler>();
    std::map<std::string,std::shared_ptr<PySysLinkBase::IBlockFactory>> blockFactories;
    for (auto &dir: plugin_dirs) {
        PySysLinkBase::BlockTypeSupportPluginLoader loader;
        auto factories = loader.LoadPlugins(dir);
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

    // 5) Read simulation options from YAML
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
    
    auto simOpts = std::make_shared<PySysLinkBase::SimulationOptions>();
    try
    {
        simOpts->startTime = opts["StartTime"].as<double>();
        simOpts->stopTime  = opts["StopTime"].as<double>();
        simOpts->runInNaturalTime = opts["RunInNaturalTime"].as<bool>();
        simOpts->naturalTimeSpeedMultiplier = opts["NaturalTimeSpeedMultiplier"].as<double>();

        for (const auto &entry : opts["BlockIdsInputOrOutputAndIndexesToLog"]) {
            simOpts->blockIdsInputOrOutputAndIndexesToLog.push_back({
                entry[0].as<std::string>(),
                entry[1].as<std::string>(),
                entry[2].as<int>()
            });
        }
        // Solvers configuration as a nested map
        for (const auto &outer : opts["SolversConfiguration"]) {
            std::map<std::string, PySysLinkBase::ConfigurationValue> inner;
            for (const auto &inner_pair : outer.second) {
                const std::string &k = inner_pair.first.as<std::string>();
                inner[k] = PySysLinkBase::ModelParser::YamlToConfigurationValue(inner_pair.second);
            }
            simOpts->solversConfiguration[outer.first.as<std::string>()] = inner;
        }
    }
    catch (const YAML::Exception &e)
    {
        std::cerr << "Error while parsing config YAML file" << std::endl;
        throw;
    }
    

    // 6) Run the simulation
    PySysLinkBase::SimulationManager mgr(model, simOpts);
    auto output = mgr.RunSimulation();

    // 7) Write CSV: one file per logged signal
    std::ofstream out(program.get<std::string>("output_csv"));
    out << "time,signal_name,value\n";
    for (auto &cat : output->signals) {
        for (auto &sig_pair : cat.second) {
            auto hist = sig_pair.second->TryCastToTyped<std::complex<double>>();
            for (size_t i = 0; i < hist->times.size(); ++i) {
                out << hist->times[i] << ","
                    << cat.first << "/" << sig_pair.first << ","
                    << hist->values[i] << "\n";
            }
        }
    }

    std::cout << "Simulation complete, output written to "
              << program.get<std::string>("output_csv") << "\n";
    return 0;
}
