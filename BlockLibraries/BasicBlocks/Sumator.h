#ifndef BLOCK_LIBRARIES_BASIC_BLOCKS_SUMATOR
#define BLOCK_LIBRARIES_BASIC_BLOCKS_SUMATOR

#include <BlockTypes/BasicCpp/SimulationBlock.h>
#include <PySysLinkBase/SampleTime.h>

namespace BlockLibraries::BasicBlocks
{
    class Sumator : public BlockTypes::BasicCpp::SimulationBlock
    {
        private:
            std::vector<double> gains;
            std::vector<BlockTypes::BasicCpp::SampleTime> sampleTimes;
        public:
            Sumator(std::map<std::string, BlockTypes::BasicCpp::ConfigurationValue> configurationValues);
            const std::vector<BlockTypes::BasicCpp::SampleTime>& GetSampleTimes() const;
            const int GetInputPortAmmount() const;
            const int GetOutputPortAmmount() const;
            const std::vector<bool> InputsHasDirectFeedthrough() const;

            std::vector<double> CalculateOutputs(const std::vector<double> inputs, BlockTypes::BasicCpp::SampleTime sampleTime);
    };
} // namespace BasicBlocks


#endif /* BLOCK_LIBRARIES_BASIC_BLOCKS_SUMATOR */
