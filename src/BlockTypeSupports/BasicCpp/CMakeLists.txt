# Define a library
add_library(BasicCpp STATIC
    SimulationBlock.cpp
    BlockWithoutContinuousStates.cpp
)

target_include_directories(BasicCpp PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(BasicCpp PRIVATE PySysLinkBase)

# Include directories for header files
target_include_directories(BasicCpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})