#ifndef BLOCK_LIBRARIES_BASIC_BLOCKS_DISPLAY
#define BLOCK_LIBRARIES_BASIC_BLOCKS_DISPLAY


#include <BlockTypeSupports/BasicCpp/SimulationBlock.h>
#include <PySysLinkBase/SampleTime.h>

namespace BlockLibraries::BasicBlocks
{
    class Display : public BlockTypeSupports::BasicCpp::SimulationBlock
    {
        private:
            std::vector<PySysLinkBase::SampleTime> sampleTimes;
        public:
            Display();

            const std::vector<PySysLinkBase::SampleTime>& GetSampleTimes() const;

            const std::vector<std::unique_ptr<PySysLinkBase::OutputPort>>& ComputeOutputsOfBlock(PySysLinkBase::SampleTime sampleTime);
    };
} // namespace BasicBlocks


#endif /* BLOCK_LIBRARIES_BASIC_BLOCKS_DISPLAY */
