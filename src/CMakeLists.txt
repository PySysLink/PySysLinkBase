# Define a library
add_library(PySysLinkBase SHARED
    ContinuousState.cpp
    SimulationModel.cpp
    ModelParser.cpp
    ISimulationBlock.cpp
    SampleTime.cpp
    BlockTypeSupportPlugingLoader.cpp
    SimulationManager.cpp
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

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
FetchContent_MakeAvailable(yaml-cpp)

add_dependencies(PySysLinkBase yaml-cpp)
target_link_libraries(PySysLinkBase PUBLIC yaml-cpp) # The library or executable that require yaml-cpp library
# target_include_directories(PySysLinkBase PRIVATE ${yaml-cpp_SOURCE_DIR}/include)
target_include_directories(PySysLinkBase PUBLIC
    $<BUILD_INTERFACE:${yaml-cpp_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>  # Include path for the build tree
    $<INSTALL_INTERFACE:include>
)
message(yaml-cpp_SOURCE_DIR="${yaml-cpp_SOURCE_DIR}")
message(STATUS "yaml-cpp_SOURCE_DIR: ${yaml-cpp_SOURCE_DIR}")

# Include directories for header files
target_include_directories(PySysLinkBase
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>  # Include path for the build tree
        $<INSTALL_INTERFACE:include>                   # Include path for the installed tree
)