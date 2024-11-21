# Define a library
add_library(BasicCpp STATIC
    SampleTime.cpp
)

target_include_directories(BasicCpp PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Include directories for header files
target_include_directories(BasicCpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})