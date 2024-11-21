#ifndef SRC_BLOCK_TYPES_BASIC_CPP_SAMPLE_TIME
#define SRC_BLOCK_TYPES_BASIC_CPP_SAMPLE_TIME

#include <cmath>
#include <vector>

namespace BlockTypes::BasicCpp
{
    enum SampleTimeType
    {
        continuous,
        discrete,
        constant,
        inherited
    };

    class SampleTime 
    {
        private:
            SampleTimeType sampleTimeType;
            double discreteSampleTime;
            int continuousSampleTimeGroup = std::numeric_limits<int>::quiet_NaN();
            std::vector<SampleTimeType> supportedSampleTimeTypesForInheritance = std::vector<SampleTimeType>{};
        public:
            SampleTime(SampleTimeType sampleTimeType, double discreteSampleTime, int continuousSampleTimeGroup, std::vector<SampleTimeType> supportedSampleTimeTypesForInheritance);
            SampleTime(SampleTimeType sampleTimeType) : SampleTime(sampleTimeType, std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<int>::quiet_NaN(), std::vector<SampleTimeType>{}){}
            SampleTime(SampleTimeType sampleTimeType, double discreteSampleTime) : SampleTime(sampleTimeType, discreteSampleTime, std::numeric_limits<int>::quiet_NaN(), std::vector<SampleTimeType>{}){}
            SampleTime(SampleTimeType sampleTimeType, int continuousSampleTimeGroup) : SampleTime(sampleTimeType, std::numeric_limits<double>::quiet_NaN(), continuousSampleTimeGroup, std::vector<SampleTimeType>{}){}
            SampleTime(SampleTimeType sampleTimeType, std::vector<SampleTimeType> supportedSampleTimeTypesForInheritance) : SampleTime(sampleTimeType, std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<int>::quiet_NaN(), supportedSampleTimeTypesForInheritance){}
            
            const SampleTimeType& GetSampleTimeType() const;
            const double GetDiscreteSampleTime() const;
            const int GetContinuousSampleTimeGroup() const;
            const std::vector<SampleTimeType> GetSupportedSampleTimeTypesForInheritance() const;
    };
}



#endif /* SRC_BLOCK_TYPES_BASIC_CPP_SAMPLE_TIME */
