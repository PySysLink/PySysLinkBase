cmake_minimum_required(VERSION 3.10)

# Plugin project name
project(BasicCppSupport)

# Locate PySysLinkBase
find_package(PySysLinkBase REQUIRED)




# Define the shared library
add_library(BasicCppSupport SHARED RegisterBlockFactories.cpp
            SampleTimeConversion.cpp
            SimulationBlockCpp.cpp
            BlockFactoryCpp.cpp
            RegisterBlockFactories.cpp)

# target_link_libraries(BasicCppSupport PRIVATE PySysLinkBase)
# target_link_libraries(BasicCppSupport PRIVATE BasicCpp)
# target_link_libraries(BasicCppSupport PRIVATE BasicBlocks)

# Include directories for header files
target_include_directories(BasicCppSupport PUBLIC ${PySysLinkBase_INCLUDE_DIRS})
target_include_directories(BasicCppSupport PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
