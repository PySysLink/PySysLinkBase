#include "ISimulationBlock.h"
#include <algorithm>

namespace PySysLinkBase
{
    ISimulationBlock::ISimulationBlock(std::map<std::string, ConfigurationValue> blockConfiguration)
    {
        this->name = ConfigurationValueManager::TryGetConfigurationValue<std::string>("Name", blockConfiguration);
        this->id = ConfigurationValueManager::TryGetConfigurationValue<std::string>("Id", blockConfiguration);
    }

    const std::string ISimulationBlock::GetId() const
    {
        return this->id;
    }
    const std::string ISimulationBlock::GetName() const
    {
        return this->name;
    }

    ISimulationBlock& ISimulationBlock::FindBlockById(std::string id, const std::vector<std::unique_ptr<ISimulationBlock>>& blocksToFind)
    {
        auto it = std::find_if(blocksToFind.begin(), blocksToFind.end(), [&id](const std::unique_ptr<ISimulationBlock>& block) {return block->GetId() == id;});

        if (it != blocksToFind.end())
        {
            int index = std::distance(blocksToFind.begin(), it);
            return *blocksToFind[index];
        }
        else
        {
            throw std::out_of_range("Block with id: " + id + " not found in provided set.");
        }
    }
} // namespace PySysLinkBase
