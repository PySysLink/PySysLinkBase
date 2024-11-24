cmake_minimum_required(VERSION 3.10)

# Plugin project name
project(BasicCppSupport)

# Define the shared library
add_library(BasicCppSupport SHARED RegisterBlockFactories.cpp)

target_include_directories(BasicCppSupport PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(BasicCppSupport PRIVATE PySysLinkBase)
target_link_libraries(BasicCppSupport PRIVATE BasicCpp)
target_link_libraries(BasicCppSupport PRIVATE BasicBlocks)

# Include directories for header files
target_include_directories(BasicCppSupport PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
