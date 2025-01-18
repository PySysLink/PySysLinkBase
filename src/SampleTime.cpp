#include "SampleTime.h"
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include "spdlog/fmt/fmt.h"
#include "spdlog/fmt/bundled/format.h"

namespace PySysLinkBase
{
    SampleTime::SampleTime(SampleTimeType sampleTimeType, double discreteSampleTime, int continuousSampleTimeGroup, std::vector<SampleTimeType> supportedSampleTimeTypesForInheritance, std::vector<std::shared_ptr<SampleTime>> multirateSampleTimes,
    int inputMultirateInheritedSampleTimeIndex, int outputMultirateInheritedSampleTimeIndex)
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
                std::cout << "Jaja hemen agian" << std::endl;
                throw std::invalid_argument("You must specify supportedSampleTimeTypesForInheritance for inherited sample time type.");
            }
            if (std::find(supportedSampleTimeTypesForInheritance.begin(), supportedSampleTimeTypesForInheritance.end(), SampleTimeType::inherited) != supportedSampleTimeTypesForInheritance.end())
            {
                throw std::invalid_argument("supportedSampleTimeTypesForInheritance can not contain inherited sample time, it can not be resolved.");
            }
            this->supportedSampleTimeTypesForInheritance = supportedSampleTimeTypesForInheritance;
        }
        else if (sampleTimeType == SampleTimeType::multirate)
        {
            if (multirateSampleTimes.empty())
            {
                throw std::invalid_argument("You must specify multirateSampleTimes for multirate sample time type.");
            }
            int inheritedCount = 0;
            for (const auto& sampleTime : multirateSampleTimes)
            {
                if (sampleTime->GetSampleTimeType() == SampleTimeType::inherited)
                {
                    inheritedCount++;
                }
            }
            if (inheritedCount > 2)
            {
                throw std::invalid_argument("Multirate sample time can not have more than 2 inherited sample times.");
            }
            else if (inheritedCount == 2)
            {
                if (std::isnan(inputMultirateInheritedSampleTimeIndex) || std::isnan(outputMultirateInheritedSampleTimeIndex))
                {
                    throw std::invalid_argument("You must specify inputMultirateInheritedSampleTimeIndex and outputMultirateInheritedSampleTimeIndex for multirate sample time type with 2 inherited sample times.");
                }
            }
            else if (inheritedCount == 1)
            {
                if (std::isnan(inputMultirateInheritedSampleTimeIndex) && std::isnan(outputMultirateInheritedSampleTimeIndex))
                {
                    throw std::invalid_argument("You must specify inputMultirateInheritedSampleTimeIndex or outputMultirateInheritedSampleTimeIndex for multirate sample time type with 1 inherited sample time.");
                }
            }
            this->multirateSampleTimes = multirateSampleTimes;
            this->inputMultirateInheritedSampleTimeIndex = inputMultirateInheritedSampleTimeIndex;
            this->outputMultirateInheritedSampleTimeIndex = outputMultirateInheritedSampleTimeIndex;
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
        if (this->sampleTimeType != SampleTimeType::inherited)
        {
            throw std::out_of_range("Sample time types distinct to inherited does not have continuous supportedSampleTimeTypesForInheritance, check before accessing");
        }
        else
        {
            return this->supportedSampleTimeTypesForInheritance;
        }
    }

    const std::vector<std::shared_ptr<SampleTime>> SampleTime::GetMultirateSampleTimes() const
    {
        if (this->sampleTimeType != SampleTimeType::multirate)
        {
            throw std::out_of_range("Sample time types distinct to multirate does not have continuous multirateSampleTimes, check before accessing");
        }
        else
        {
            return this->multirateSampleTimes;
        }
    }

    const void SampleTime::SetMultirateSampleTimeInIndex(std::shared_ptr<SampleTime> multirateSampleTime, int index)
    {
        if (this->sampleTimeType != SampleTimeType::multirate)
        {
            throw std::out_of_range("Sample time types distinct to multirate does not have continuous multirateSampleTimes, check before accessing");
        }
        else
        {
            this->multirateSampleTimes[index] = multirateSampleTime;
        }
    }

    
    const bool SampleTime::HasMultirateInheritedSampleTime() const
    {
        if (this->sampleTimeType != SampleTimeType::multirate)
        {
            throw std::out_of_range("Sample time types distinct to multirate can not be checked this way, check before accessing");
        }
        else
        {
            bool hasInherited = false;
            for (const auto& sampleTime : this->multirateSampleTimes)
            {
                if (sampleTime->GetSampleTimeType() == SampleTimeType::inherited)
                {
                    hasInherited = true;
                    break;
                }
            }
            return hasInherited;
        }
    }

    const int SampleTime::GetInputMultirateInheritedSampleTimeIndex() const
    {
        if (this->sampleTimeType != SampleTimeType::multirate)
        {
            throw std::out_of_range("Sample time types distinct to multirate does not have continuous inputMultirateInheritedSampleTimeIndex, check before accessing");
        }
        else
        {
            return this->inputMultirateInheritedSampleTimeIndex;
        }
    }

    const int SampleTime::GetOutputMultirateInheritedSampleTimeIndex() const
    {
        if (this->sampleTimeType != SampleTimeType::multirate)
        {
            throw std::out_of_range("Sample time types distinct to multirate does not have continuous outputMultirateInheritedSampleTimeIndex, check before accessing");
        }
        else
        {
            return this->outputMultirateInheritedSampleTimeIndex;
        }
    }


    std::string SampleTime::SampleTimeTypeString(SampleTimeType sampleTimeType)
    {
        switch (sampleTimeType) 
        {
            case PySysLinkBase::continuous: return "Continuous";
            case PySysLinkBase::discrete:   return "Discrete";
            case PySysLinkBase::constant:   return "Constant";
            case PySysLinkBase::inherited:  return "Inherited";
            case PySysLinkBase::multirate:  return "Multirate";
            default: return "Unknown";
        }
    }
} // namespace PySysLinkBase

