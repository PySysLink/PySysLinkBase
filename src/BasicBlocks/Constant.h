#ifndef BLOCK_LIBRARIES_BASIC_BLOCKS_CONSTANT
#define BLOCK_LIBRARIES_BASIC_BLOCKS_CONSTANT

#include <BlockTypeSupports/BasicCpp/SimulationBlock.h>
#include <PySysLinkBase/SampleTime.h>

namespace BlockLibraries::BasicBlocks
{
    class Constant : public BlockTypeSupports::BasicCpp::SimulationBlock
    {
        private:
            double value;
            std::vector<PySysLinkBase::SampleTime> sampleTimes;
        public:
            Constant(double value);

            const std::vector<PySysLinkBase::SampleTime>& GetSampleTimes() const;

            const std::vector<std::unique_ptr<PySysLinkBase::OutputPort>>& ComputeOutputsOfBlock(PySysLinkBase::SampleTime sampleTime);
    };
} // namespace BasicBlocks

#endif /* BLOCK_LIBRARIES_BASIC_BLOCKS_CONSTANT */
