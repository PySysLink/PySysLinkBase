# Define a library
add_library(PySysLinkBase STATIC
    ContinuousState.cpp
    SimulationModel.cpp
    PortsAndSignalValues/InputPort.cpp 
    PortsAndSignalValues/OutputPort.cpp 
    PortsAndSignalValues/Port.cpp 
)


# Include directories for header files
target_include_directories(PySysLinkBase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})