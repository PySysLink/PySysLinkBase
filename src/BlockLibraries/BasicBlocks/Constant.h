#ifndef SRC_BASIC_BLOCKS_CONSTANT
#define SRC_BASIC_BLOCKS_CONSTANT

#include <BlockTypes/BasicCpp/SimulationBlock.h>
#include <BlockTypes/BasicCpp/SampleTime.h>
#include <BlockTypes/BasicCpp/ConfigurationValue.h>

namespace BlockLibraries::BasicBlocks
{
    class Constant : public BlockTypes::BasicCpp::SimulationBlock
    {
        private:
            double value;
            std::vector<BlockTypes::BasicCpp::SampleTime> sampleTimes;
        public:
            Constant(std::map<std::string, BlockTypes::BasicCpp::ConfigurationValue> configurationValues);
            const std::vector<BlockTypes::BasicCpp::SampleTime>& GetSampleTimes() const;
            const int GetInputPortAmmount() const;
            const int GetOutputPortAmmount() const;
            const std::vector<bool> InputsHasDirectFeedthrough() const;

            std::vector<double> CalculateOutputs(const std::vector<double> inputs, BlockTypes::BasicCpp::SampleTime sampleTime);
    };
} // namespace BasicBlocks

#endif /* SRC_BASIC_BLOCKS_CONSTANT */
