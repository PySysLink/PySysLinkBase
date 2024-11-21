#ifndef SRC_BLOCK_TYPES_BASIC_CPP_SIMULATION_BLOCK
#define SRC_BLOCK_TYPES_BASIC_CPP_SIMULATION_BLOCK

#include <map>
#include <vector>
#include <string>
#include "ConfigurationValue.h"
#include "SampleTime.h"
#include <stdexcept>

namespace BlockTypes::BasicCpp
{
    class SimulationBlock
    {
        protected:
        template <typename T> 
        T TryGetConfigurationValue(std::string keyName, std::map<std::string, ConfigurationValue> configurationValues)
        {
            ConfigurationValue findedValue;
            auto it = configurationValues.find(keyName);
            if (it == configurationValues.end()) {
                throw std::invalid_argument("Key name: " + keyName + " not found in configuration.");
            } else {
                findedValue = it->second;
            }
            try
            {
                return std::get<T>(findedValue);
            }
            catch (std::bad_variant_access const& ex)
            {
                throw std::invalid_argument("Configuration option of key: " + keyName + " was not of expected type: " + ex.what());
            }
        }

        public:
        virtual const std::vector<SampleTime>& GetSampleTimes() const = 0;
        virtual const int GetInputPortAmmount() const = 0;
        virtual const int GetOutputPortAmmount() const = 0;
        virtual const std::vector<bool> InputsHasDirectFeedthrough() const = 0;

        virtual std::vector<double> CalculateOutputs(const std::vector<double> inputs, SampleTime sampleTime) = 0;
    };
} // namespace BlockTypes::BasicCpp


#endif /* SRC_BLOCK_TYPES_BASIC_CPP_SIMULATION_BLOCK */
