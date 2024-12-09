#include "PortLink.h"

namespace PySysLinkBase
{
    PortLink PortLink::ParseFromConfig(std::map<std::string, ConfigurationValue> linkConfiguration, const std::vector<std::shared_ptr<ISimulationBlock>>& blocks)
    {
        std::string sourceBlockId = ConfigurationValueManager::TryGetConfigurationValue<std::string>("SourceBlockId", linkConfiguration);
        int sourcePortIdx = ConfigurationValueManager::TryGetConfigurationValue<int>("SourcePortIdx", linkConfiguration);
        std::string destinationBlockId = ConfigurationValueManager::TryGetConfigurationValue<std::string>("DestinationBlockId", linkConfiguration);
        int destinationPortIdx = ConfigurationValueManager::TryGetConfigurationValue<int>("DestinationPortIdx", linkConfiguration);

        std::shared_ptr<ISimulationBlock> sourceBlock = ISimulationBlock::FindBlockById(sourceBlockId, blocks);
        std::shared_ptr<ISimulationBlock> destinationBlock = ISimulationBlock::FindBlockById(destinationBlockId, blocks);

        return PortLink(sourceBlock, destinationBlock, sourcePortIdx, destinationPortIdx);
    }
} // namespace PySysLinkBase
