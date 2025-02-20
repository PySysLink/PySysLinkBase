#ifndef PY_SYS_LINK_BASE_SAMPLE_TIME
#define PY_SYS_LINK_BASE_SAMPLE_TIME

#include <cmath>
#include <vector>
#include <limits>
#include <string>
#include <memory>

namespace PySysLinkBase
{
    enum SampleTimeType
    {
        continuous,
        discrete,
        constant,
        inherited,
        multirate
    };

    class SampleTime 
    {
        private:
            SampleTimeType sampleTimeType;
            double discreteSampleTime;
            int continuousSampleTimeGroup = -1;
            std::vector<SampleTimeType> supportedSampleTimeTypesForInheritance = std::vector<SampleTimeType>{};
            std::vector<std::shared_ptr<SampleTime>> multirateSampleTimes = {};
            int inputMultirateSampleTimeIndex = -1;
            int outputMultirateSampleTimeIndex = -1;
        public:
            SampleTime(SampleTimeType sampleTimeType, 
                        double discreteSampleTime, int continuousSampleTimeGroup, std::vector<SampleTimeType> supportedSampleTimeTypesForInheritance, std::vector<std::shared_ptr<SampleTime>> multirateSampleTimes, 
                        int inputMultirateSampleTimeIndex = -1, int outputMultirateSampleTimeIndex = -1);

            SampleTime(SampleTimeType sampleTimeType) : SampleTime(sampleTimeType, std::numeric_limits<double>::quiet_NaN(), -1, std::vector<SampleTimeType>{}, {}){}
            SampleTime(SampleTimeType sampleTimeType, double discreteSampleTime) : SampleTime(sampleTimeType, discreteSampleTime, -1, std::vector<SampleTimeType>{}, {}){}
            SampleTime(SampleTimeType sampleTimeType, int continuousSampleTimeGroup) : SampleTime(sampleTimeType, std::numeric_limits<double>::quiet_NaN(), continuousSampleTimeGroup, std::vector<SampleTimeType>{}, {}){}
            SampleTime(SampleTimeType sampleTimeType, std::vector<SampleTimeType> supportedSampleTimeTypesForInheritance) : SampleTime(sampleTimeType, std::numeric_limits<double>::quiet_NaN(), -1, supportedSampleTimeTypesForInheritance, {}){}
            SampleTime(SampleTimeType sampleTimeType, std::vector<std::shared_ptr<SampleTime>> multirateSampleTimes) : SampleTime(sampleTimeType, std::numeric_limits<double>::quiet_NaN(), -1, {}, multirateSampleTimes){}
            SampleTime(SampleTimeType sampleTimeType, std::vector<std::shared_ptr<SampleTime>> multirateSampleTimes, int inputMultirateSampleTimeIndex, int outputMultirateSampleTimeIndex) 
            : SampleTime(sampleTimeType, std::numeric_limits<double>::quiet_NaN(), -1, {}, multirateSampleTimes, inputMultirateSampleTimeIndex, outputMultirateSampleTimeIndex){}
            
            const SampleTimeType& GetSampleTimeType() const;
            const double GetDiscreteSampleTime() const;
            const int GetContinuousSampleTimeGroup() const;
            const std::vector<SampleTimeType> GetSupportedSampleTimeTypesForInheritance() const;
            const std::vector<std::shared_ptr<SampleTime>> GetMultirateSampleTimes() const;
            const void SetMultirateSampleTimeInIndex(std::shared_ptr<SampleTime> multirateSampleTime, int index);
            const bool HasMultirateInheritedSampleTime() const;
            const int GetInputMultirateSampleTimeIndex() const;
            const int GetOutputMultirateSampleTimeIndex() const;
            const bool IsInputMultirateInherited() const;
            const bool IsOutputMultirateInherited() const;

            static std::string SampleTimeTypeString(SampleTimeType sampleTimeType);
    };
}

#endif /* PY_SYS_LINK_BASE_SAMPLE_TIME */
