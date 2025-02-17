# Define a library
add_library(PySysLinkBase STATIC
    ContinuousAndOde/ContinuousState.cpp
    SimulationModel.cpp
    ModelParser.cpp
    ISimulationBlock.cpp
    SampleTime.cpp
    BlockTypeSupportPlugingLoader.cpp
    SimulationManager.cpp
    PortLink.cpp
    SpdlogManager.cpp
    BlockEventsHandler.cpp
    ContinuousAndOde/BasicOdeSolver.cpp
    ContinuousAndOde/EulerForwardStepSolver.cpp
    ContinuousAndOde/SolverFactory.cpp

    PortsAndSignalValues/InputPort.cpp 
    PortsAndSignalValues/OutputPort.cpp 
    PortsAndSignalValues/Port.cpp 
)

set_target_properties(PySysLinkBase PROPERTIES POSITION_INDEPENDENT_CODE ON)


include(FetchContent)

FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG yaml-cpp-0.6.0 # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
option(YAML_CPP_BUILD_TOOLS "Enable testing and parse tools" OFF)
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(yaml-cpp)

add_dependencies(PySysLinkBase yaml-cpp)
target_link_libraries(PySysLinkBase PUBLIC yaml-cpp) # The library or executable that require yaml-cpp library


FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.15.0
)

FetchContent_MakeAvailable(spdlog)

include(GNUInstallDirs)

# Ensure spdlog is installed with your project
install(TARGETS spdlog
    EXPORT SpdlogTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Ensure spdlog is part of your export
install(EXPORT SpdlogTargets
    FILE SpdlogConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/spdlog
)

add_dependencies(PySysLinkBase spdlog::spdlog)
target_link_libraries(PySysLinkBase PRIVATE spdlog::spdlog)


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