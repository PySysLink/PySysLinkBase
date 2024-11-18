#ifndef BLOCK_LIBRARIES_BASIC_BLOCKS_SUMATOR
#define BLOCK_LIBRARIES_BASIC_BLOCKS_SUMATOR

#include <BlockTypeSupports/BasicCpp/SimulationBlock.h>
#include <PySysLinkBase/SampleTime.h>

namespace BlockLibraries::BasicBlocks
{
    class Sumator : public BlockTypeSupports::BasicCpp::SimulationBlock
    {
        private:
            std::vector<double> gains;
            std::vector<PySysLinkBase::SampleTime> sampleTimes;
        public:
            Sumator(std::vector<double> gains);

            const std::vector<PySysLinkBase::SampleTime>& GetSampleTimes() const;

            const std::vector<std::unique_ptr<PySysLinkBase::OutputPort>>& ComputeOutputsOfBlock(PySysLinkBase::SampleTime sampleTime);
    };
} // namespace BasicBlocks


#endif /* BLOCK_LIBRARIES_BASIC_BLOCKS_SUMATOR */
