#ifndef PY_SYS_LINK_BASE_SAMPLE_TIME
#define PY_SYS_LINK_BASE_SAMPLE_TIME

#include <cmath>
#include <vector>
#include <limits>
#include <string>

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
            int continuousSampleTimeGroup = std::numeric_limits<int>::quiet_NaN();
            std::vector<SampleTimeType> supportedSampleTimeTypesForInheritance = std::vector<SampleTimeType>{};
            std::vector<SampleTime> multirateSampleTimes = {};
        public:
            SampleTime(SampleTimeType sampleTimeType, double discreteSampleTime, int continuousSampleTimeGroup, std::vector<SampleTimeType> supportedSampleTimeTypesForInheritance, std::vector<SampleTime> multirateSampleTimes);
            SampleTime(SampleTimeType sampleTimeType) : SampleTime(sampleTimeType, std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<int>::quiet_NaN(), std::vector<SampleTimeType>{}, {}){}
            SampleTime(SampleTimeType sampleTimeType, double discreteSampleTime) : SampleTime(sampleTimeType, discreteSampleTime, std::numeric_limits<int>::quiet_NaN(), std::vector<SampleTimeType>{}, {}){}
            SampleTime(SampleTimeType sampleTimeType, int continuousSampleTimeGroup) : SampleTime(sampleTimeType, std::numeric_limits<double>::quiet_NaN(), continuousSampleTimeGroup, std::vector<SampleTimeType>{}, {}){}
            SampleTime(SampleTimeType sampleTimeType, std::vector<SampleTimeType> supportedSampleTimeTypesForInheritance) : SampleTime(sampleTimeType, std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<int>::quiet_NaN(), supportedSampleTimeTypesForInheritance, {}){}
            SampleTime(SampleTimeType sampleTimeType, std::vector<SampleTime> multirateSampleTimes) : SampleTime(sampleTimeType, std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<int>::quiet_NaN(), {}, multirateSampleTimes){}
            
            const SampleTimeType& GetSampleTimeType() const;
            const double GetDiscreteSampleTime() const;
            const int GetContinuousSampleTimeGroup() const;
            const std::vector<SampleTimeType> GetSupportedSampleTimeTypesForInheritance() const;

            static std::string SampleTimeTypeString(SampleTimeType sampleTimeType);
    };
}

#endif /* PY_SYS_LINK_BASE_SAMPLE_TIME */
