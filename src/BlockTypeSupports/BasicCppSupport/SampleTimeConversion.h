#ifndef SRC_BLOCK_TYPE_SUPPORTS_BASIC_CPP_SUPPORT_SAMPLE_TIME_CONVERSION
#define SRC_BLOCK_TYPE_SUPPORTS_BASIC_CPP_SUPPORT_SAMPLE_TIME_CONVERSION

#include <PySysLinkBase/SampleTime.h>
#include <BlockTypes/BasicCpp/SampleTime.h>

namespace BlockTypeSupports::BasicCppSupport
{
    class SampleTimeConversion
    {
        public:
        const static PySysLinkBase::SampleTime CppSampleTimeToPySysLink(BlockTypes::BasicCpp::SampleTime sampleTime);
        const static BlockTypes::BasicCpp::SampleTime PySysLinkTimeToCpp(PySysLinkBase::SampleTime sampleTime);
        const static PySysLinkBase::SampleTimeType CppSampleTimeTypeToPySysLink(BlockTypes::BasicCpp::SampleTimeType sampleTimeType);
        const static BlockTypes::BasicCpp::SampleTimeType PySysLinkTimeTypeToCpp(PySysLinkBase::SampleTimeType sampleTimeType);
    };
} // namespace name


#endif /* SRC_BLOCK_TYPE_SUPPORTS_BASIC_CPP_SUPPORT_SAMPLE_TIME_CONVERSION */
