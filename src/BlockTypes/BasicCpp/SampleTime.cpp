#include "SampleTime.h"
#include <stdexcept>
#include <algorithm>

namespace BlockTypes::BasicCpp
{
    SampleTime::SampleTime(SampleTimeType sampleTimeType, double discreteSampleTime, int continuousSampleTimeGroup, std::vector<SampleTimeType> supportedSampleTimeTypesForInheritance)
    {
        this->sampleTimeType = sampleTimeType;
        if (sampleTimeType == SampleTimeType::discrete)
        {
            if (std::isnan(discreteSampleTime))
            {
                throw std::invalid_argument("You must specify discreteSampleTime for discrete sample time type.");
            }
            this->discreteSampleTime = discreteSampleTime;
        }
        else if (sampleTimeType == SampleTimeType::continuous)
        {
            if (std::isnan(continuousSampleTimeGroup))
            {
                throw std::invalid_argument("You must specify continuousSampleTimeGroup for continuous sample time type.");
            }
            this->continuousSampleTimeGroup = continuousSampleTimeGroup;
        }
        else if (sampleTimeType == SampleTimeType::inherited)
        {
            if (supportedSampleTimeTypesForInheritance.empty())
            {
                throw std::invalid_argument("You must specify supportedSampleTimeTypesForInheritance for inherited sample time type.");
                if (std::find(supportedSampleTimeTypesForInheritance.begin(), supportedSampleTimeTypesForInheritance.end(), SampleTimeType::inherited) != supportedSampleTimeTypesForInheritance.end())
                {
                    throw std::invalid_argument("supportedSampleTimeTypesForInheritance can not contain inherited sample time, it can not be resolved.");
                }
            }
            this->supportedSampleTimeTypesForInheritance = supportedSampleTimeTypesForInheritance;
        }
    }

    const SampleTimeType& SampleTime::GetSampleTimeType() const
    {
        return this->sampleTimeType;
    }

    const double SampleTime::GetDiscreteSampleTime() const
    {
        if (this->sampleTimeType != SampleTimeType::discrete)
        {
            throw std::out_of_range("Sample time types distinct to discrete does not have discreteSampleTime, check before accessing");
        }
        else
        {
            return this->discreteSampleTime;
        }
    }

    const int SampleTime::GetContinuousSampleTimeGroup() const
    {
        if (this->sampleTimeType != SampleTimeType::continuous)
        {
            throw std::out_of_range("Sample time types distinct to continuous does not have continuousSampleTimeGroup, check before accessing");
        }
        else
        {
            return this->continuousSampleTimeGroup;
        }
    }
    
    const std::vector<SampleTimeType> SampleTime::GetSupportedSampleTimeTypesForInheritance() const
    {
        if (this->sampleTimeType != SampleTimeType::continuous)
        {
            throw std::out_of_range("Sample time types distinct to inherited does not have continuous supportedSampleTimeTypesForInheritance, check before accessing");
        }
        else
        {
            return this->supportedSampleTimeTypesForInheritance;
        }
    }
} // namespace BlockTypes::BasicCpp
