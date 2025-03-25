#include "PySysLinkBase/SimulationModel.h"
#include "PySysLinkBase/ModelParser.h"
#include "PySysLinkBase/ISimulationBlock.h"
#include "PySysLinkBase/PortLink.h"
#include "PySysLinkBase/IBlockFactory.h"
#include "PySysLinkBase/BlockTypeSupportPluginLoader.h"
#include "PySysLinkBase/SimulationManager.h"
#include "PySysLinkBase/SpdlogManager.h"
#include "PySysLinkBase/BlockEventsHandler.h"
#include "PySysLinkBase/SimulationOptions.h"
#include "PySysLinkBase/SimulationOutput.h"
#include <map>
#include <iostream>
#include <chrono>
#include <thread>
#include <signal.h>

bool interrupt = false;

void ctr_c_handler(int s)
{
    printf("Caught signal %d\n",s);
    interrupt = true; 

}

int main() {
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = ctr_c_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);



    PySysLinkBase::SpdlogManager::ConfigureDefaultLogger();
    PySysLinkBase::SpdlogManager::SetLogLevel(PySysLinkBase::LogLevel::debug);
    
    std::shared_ptr<PySysLinkBase::IBlockEventsHandler> blockEventsHandler = std::make_shared<PySysLinkBase::BlockEventsHandler>();

    std::unique_ptr<PySysLinkBase::BlockTypeSupportPluginLoader> pluginLoader = std::make_unique<PySysLinkBase::BlockTypeSupportPluginLoader>();
    PySysLinkBase::SpdlogManager::SetLogLevel(PySysLinkBase::LogLevel::debug);

    std::map<std::string, std::shared_ptr<PySysLinkBase::IBlockFactory>> blockFactories = pluginLoader->LoadPlugins("/usr/local/lib");
    
    PySysLinkBase::SpdlogManager::SetLogLevel(PySysLinkBase::LogLevel::debug);

    std::shared_ptr<PySysLinkBase::SimulationModel> simulationModel = PySysLinkBase::ModelParser::ParseFromYaml("../Tests/system1_continuous.yaml", blockFactories, blockEventsHandler);

    std::vector<std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>>> blockChains = simulationModel->GetDirectBlockChains();
    

    std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>> orderedBlocks = simulationModel->OrderBlockChainsOntoFreeOrder(blockChains);
  

    simulationModel->PropagateSampleTimes();

    PySysLinkBase::SpdlogManager::SetLogLevel(PySysLinkBase::LogLevel::debug);

    std::shared_ptr<PySysLinkBase::SimulationOptions> simulationOptions = std::make_shared<PySysLinkBase::SimulationOptions>();
    simulationOptions->startTime = 0.0;
    simulationOptions->stopTime = 10.0;
    simulationOptions->runInNaturalTime = true;
    simulationOptions->naturalTimeSpeedMultiplier = 1;
    simulationOptions->blockIdsInputOrOutputAndIndexesToLog = {{"const1", "output", 0}, {"integrator1", "output", 0}};
    simulationOptions->solversConfiguration = {
            { "default", {
                { "Type", PySysLinkBase::ConfigurationValue(std::string("odeint")) },
                { "ControlledSolver", PySysLinkBase::ConfigurationValue(std::string("runge_kutta_fehlberg78")) },
                { "AbsoluteTolerance", PySysLinkBase::ConfigurationValue(1e-10) },
                { "RelativeTolerance", PySysLinkBase::ConfigurationValue(1e-10) }
            }}
        };

    std::unique_ptr<PySysLinkBase::SimulationManager> simulationManager = std::make_unique<PySysLinkBase::SimulationManager>(simulationModel, simulationOptions);
    
    auto simulationStartTime = std::chrono::system_clock::now();

    double nextTimeHit = 0.0;
    while (!interrupt)
    {
        auto targetTime = simulationStartTime + std::chrono::duration<double>(nextTimeHit);

        std::this_thread::sleep_until(targetTime);
        auto actualTime = std::chrono::system_clock::now();
        auto elapsedRealTime = std::chrono::duration<double>(actualTime - simulationStartTime).count();
        nextTimeHit = simulationManager->RunSimulationStep();
    }

    std::shared_ptr<PySysLinkBase::SimulationOutput> simulationOutput = simulationManager->GetSimulationOutput();
    
    std::vector<double> continuousValues = simulationOutput->signals["Displays"]["display1"]->TryCastToTyped<double>()->values;
    std::vector<double> continuousTimes = simulationOutput->signals["Displays"]["display1"]->TryCastToTyped<double>()->times;
    for (int i = 0; i < continuousValues.size(); i++)
    {
        std::cout << continuousTimes[i] << ": " << continuousValues[i] << std::endl;
    }
    
    std::vector<double> continuousValuesLog = simulationOutput->signals["LoggedSignals"]["integrator1/output/0"]->TryCastToTyped<double>()->values;
    std::vector<double> continuousTimesLog = simulationOutput->signals["LoggedSignals"]["integrator1/output/0"]->TryCastToTyped<double>()->times;
    for (int i = 0; i < continuousValuesLog.size(); i++)
    {
        std::cout << continuousTimesLog[i] << ": " << continuousValuesLog[i] << std::endl;
    }

    return 0;
}

