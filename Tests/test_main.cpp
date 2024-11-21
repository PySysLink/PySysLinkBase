#include <PySysLinkBase/SimulationModel.h>
#include <PySysLinkBase/ISimulationBlock.h>
#include <PySysLinkBase/PortLink.h>
#include <iostream>
// #include <yaml-cpp/yaml.h>



// std::vector<Block> loadBlocks(const YAML::Node& node) {
//     std::vector<Block> blocks;
//     for (const auto& blockNode : node) {
//         std::string id = blockNode["Id"].as<std::string>();
//         std::string name = blockNode["Name"].as<std::string>();
//         std::string blockType = blockNode["BlockType"].as<std::string>();
//         std::string blockId = blockNode["BlockId"].as<std::string>();
//         int inputPorts = blockNode["InputPorts"].as<int>();
//         int outputPorts = blockNode["OutputPorts"].as<int>();
        
//         double value = 0;
//         if (blockNode["Value"]) {
//             value = blockNode["Value"].as<double>();
//         }

//         blocks.emplace_back(id, name, blockType, blockId, inputPorts, outputPorts, value);
//     }
//     return blocks;
// }

// std::vector<Link> loadLinks(const YAML::Node& node) {
//     std::vector<Link> links;
//     for (const auto& linkNode : node) {
//         std::string id = linkNode["Id"].as<std::string>();
//         std::string sourceBlock = linkNode["SourceBlock"].as<std::string>();
//         int sourcePortIdx = linkNode["SourcePortIdx"].as<int>();
//         std::string destinationBlock = linkNode["DestinationBlock"].as<std::string>();
//         int destinationPortIdx = linkNode["DestinationPortIdx"].as<int>();

//         links.emplace_back(id, sourceBlock, sourcePortIdx, destinationBlock, destinationPortIdx);
//     }
//     return links;
// }

// void ParseYaml()
// {
//     YAML::Node config = YAML::LoadFile("system1.yaml");

//     if (config["Blocks"] && config["Links"]) {
//         std::vector<Block> blocks = loadBlocks(config["Blocks"]);
//         std::vector<Link> links = loadLinks(config["Links"]);

//         // Example: Print loaded blocks and links
//         for (const auto& block : blocks) {
//             std::cout << "Block ID: " << block.id << ", Name: " << block.name
//                       << ", BlockType: " << block.blockType
//                       << ", BlockId: " << block.blockId
//                       << ", InputPorts: " << block.inputPorts
//                       << ", OutputPorts: " << block.outputPorts
//                       << ", Value: " << block.value << std::endl;
//         }

//         for (const auto& link : links) {
//             std::cout << "Link ID: " << link.id
//                       << ", Source: " << link.sourceBlock << ":" << link.sourcePortIdx
//                       << ", Destination: " << link.destinationBlock << ":" << link.destinationPortIdx
//                       << std::endl;
//         }
//     } else {
//         std::cerr << "Error: Missing 'Blocks' or 'Links' in YAML file" << std::endl;
//     }
// }

int main() {

    std::vector<std::unique_ptr<PySysLinkBase::ISimulationBlock>> simulationBlocks = {};

    std::vector<std::unique_ptr<PySysLinkBase::PortLink>> portLinks = {};

    std::unique_ptr<PySysLinkBase::SimulationModel> model =
        std::make_unique<PySysLinkBase::SimulationModel>(
            std::move(simulationBlocks),
            std::move(portLinks)
        );    
    std::cout << "End" << std::endl;


    return 0;
}

