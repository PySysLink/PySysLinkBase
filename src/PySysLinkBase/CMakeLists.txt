# Define a library
add_library(PySysLinkBase STATIC
    ContinuousState.cpp
    SimulationModel.cpp
    ModelParser.cpp
    PortsAndSignalValues/InputPort.cpp 
    PortsAndSignalValues/OutputPort.cpp 
    PortsAndSignalValues/Port.cpp 
)

include(FetchContent)

FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG yaml-cpp-0.6.0 # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
)
FetchContent_MakeAvailable(yaml-cpp)

target_link_libraries(PySysLinkBase PUBLIC yaml-cpp) # The library or executable that require yaml-cpp library
target_include_directories(PySysLinkBase PRIVATE ${yaml-cpp_SOURCE_DIR}/include)

# Include directories for header files
target_include_directories(PySysLinkBase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})