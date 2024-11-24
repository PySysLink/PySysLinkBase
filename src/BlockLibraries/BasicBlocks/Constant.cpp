#include "Constant.h"
#include <vector>
#include <stdexcept>

namespace BlockLibraries::BasicBlocks
{
    Constant::Constant(std::map<std::string, BlockTypes::BasicCpp::ConfigurationValue> configurationValues)
    {
        this->value = BlockTypes::BasicCpp::ConfigurationValueManager::TryGetConfigurationValue<double>("Value", configurationValues);

        this->sampleTimes.push_back(BlockTypes::BasicCpp::SampleTime(BlockTypes::BasicCpp::SampleTimeType::constant));
    }

    const std::vector<BlockTypes::BasicCpp::SampleTime>& Constant::GetSampleTimes() const
    {
        return this->sampleTimes;
    }

    const int Constant::GetInputPortAmmount() const
    {
        return 0;
    }
    const int Constant::GetOutputPortAmmount() const
    {
        return 1;
    }

    const std::vector<bool> Constant::InputsHasDirectFeedthrough() const 
    {
        return {};
    }

    std::vector<double> Constant::CalculateOutputs(const std::vector<double> inputs, BlockTypes::BasicCpp::SampleTime sampleTime)
    {
        return {this->value};
    }

} // namespace BlockLibraries::BasicBlocks

