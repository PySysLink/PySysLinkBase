#include "SampleTimeConversion.h"
#include <stdexcept>

namespace BlockTypeSupports::BasicCppSupport
{
    const PySysLinkBase::SampleTime SampleTimeConversion::CppSampleTimeToPySysLink(BlockTypes::BasicCpp::SampleTime sampleTime)
    {
        BlockTypes::BasicCpp::SampleTimeType sampleTimeType = sampleTime.GetSampleTimeType();
        if (sampleTimeType == BlockTypes::BasicCpp::SampleTimeType::constant)
        {
            return PySysLinkBase::SampleTime(PySysLinkBase::SampleTimeType::constant);
        }
        else if (sampleTimeType == BlockTypes::BasicCpp::SampleTimeType::discrete)
        {
            return PySysLinkBase::SampleTime(PySysLinkBase::SampleTimeType::discrete, sampleTime.GetDiscreteSampleTime());
        }
        else if (sampleTimeType == BlockTypes::BasicCpp::SampleTimeType::continuous)
        {
            return PySysLinkBase::SampleTime(PySysLinkBase::SampleTimeType::continuous, sampleTime.GetContinuousSampleTimeGroup());
        }
        else if (sampleTimeType == BlockTypes::BasicCpp::SampleTimeType::inherited)
        {
            std::vector<PySysLinkBase::SampleTimeType> supportedSampleTimeTypes = {};
            std::vector<BlockTypes::BasicCpp::SampleTimeType> supportedSampleTimeTypesCpp = sampleTime.GetSupportedSampleTimeTypesForInheritance();
            for (int i; i < supportedSampleTimeTypesCpp.size(); i++)
            {
                supportedSampleTimeTypes.push_back(SampleTimeConversion::CppSampleTimeTypeToPySysLink(supportedSampleTimeTypesCpp[i]));
            }
            return PySysLinkBase::SampleTime(PySysLinkBase::SampleTimeType::inherited, supportedSampleTimeTypes);
        }
        else 
        {
            throw std::runtime_error("Unsupported sample time type.");
        }
    }

    const BlockTypes::BasicCpp::SampleTime SampleTimeConversion::PySysLinkTimeToCpp(PySysLinkBase::SampleTime sampleTime)
    {
        PySysLinkBase::SampleTimeType sampleTimeType = sampleTime.GetSampleTimeType();
        if (sampleTimeType == PySysLinkBase::SampleTimeType::constant)
        {
            return BlockTypes::BasicCpp::SampleTime(BlockTypes::BasicCpp::SampleTimeType::constant);
        }
        else if (sampleTimeType == PySysLinkBase::SampleTimeType::discrete)
        {
            return BlockTypes::BasicCpp::SampleTime(BlockTypes::BasicCpp::SampleTimeType::discrete, sampleTime.GetDiscreteSampleTime());
        }
        else if (sampleTimeType == PySysLinkBase::SampleTimeType::continuous)
        {
            return BlockTypes::BasicCpp::SampleTime(BlockTypes::BasicCpp::SampleTimeType::continuous, sampleTime.GetContinuousSampleTimeGroup());
        }
        else if (sampleTimeType == PySysLinkBase::SampleTimeType::inherited)
        {
            std::vector<BlockTypes::BasicCpp::SampleTimeType> supportedSampleTimeTypes = {};
            std::vector<PySysLinkBase::SampleTimeType> supportedSampleTimeTypesPySysLink = sampleTime.GetSupportedSampleTimeTypesForInheritance();
            for (int i; i < supportedSampleTimeTypesPySysLink.size(); i++)
            {
                supportedSampleTimeTypes.push_back(SampleTimeConversion::PySysLinkTimeTypeToCpp(supportedSampleTimeTypesPySysLink[i]));
            }
            return BlockTypes::BasicCpp::SampleTime(BlockTypes::BasicCpp::SampleTimeType::inherited, supportedSampleTimeTypes);
        }
        else 
        {
            throw std::runtime_error("Unsupported sample time type.");
        }
    }

    const PySysLinkBase::SampleTimeType SampleTimeConversion::CppSampleTimeTypeToPySysLink(BlockTypes::BasicCpp::SampleTimeType sampleTimeType)
    {
        if (sampleTimeType == BlockTypes::BasicCpp::SampleTimeType::constant)
        {
            return PySysLinkBase::SampleTimeType::constant;
        }
        else if (sampleTimeType == BlockTypes::BasicCpp::SampleTimeType::continuous)
        {
            return PySysLinkBase::SampleTimeType::continuous;
        }
        else if (sampleTimeType == BlockTypes::BasicCpp::SampleTimeType::discrete)
        {
            return PySysLinkBase::SampleTimeType::discrete;
        }
        else if (sampleTimeType == BlockTypes::BasicCpp::SampleTimeType::inherited)
        {
            return PySysLinkBase::SampleTimeType::inherited;
        }
        else
        {
            throw std::runtime_error("Unsupported sample time type.");
        }
    }
    
    const BlockTypes::BasicCpp::SampleTimeType SampleTimeConversion::PySysLinkTimeTypeToCpp(PySysLinkBase::SampleTimeType sampleTimeType)
    {
        if (sampleTimeType == PySysLinkBase::SampleTimeType::constant)
        {
            return BlockTypes::BasicCpp::SampleTimeType::constant;
        }
        else if (sampleTimeType == PySysLinkBase::SampleTimeType::continuous)
        {
            return BlockTypes::BasicCpp::SampleTimeType::continuous;
        }
        else if (sampleTimeType == PySysLinkBase::SampleTimeType::discrete)
        {
            return BlockTypes::BasicCpp::SampleTimeType::discrete;
        }
        else if (sampleTimeType == PySysLinkBase::SampleTimeType::inherited)
        {
            return BlockTypes::BasicCpp::SampleTimeType::inherited;
        }
        else
        {
            throw std::runtime_error("Unsupported sample time type.");
        }
    }
} // namespace BlockTypeSupports::BasicCppSupport
