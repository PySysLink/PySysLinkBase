# -------------------------------------------------------------------
# Core library
# -------------------------------------------------------------------
add_library(PySysLinkBaseCore OBJECT
    SimulationModel.cpp
    ModelParser.cpp
    ISimulationBlock.cpp
    SampleTime.cpp
    BlockTypeSupportPluginLoader.cpp
    SimulationManager.cpp
    PortLink.cpp
    SpdlogManager.cpp
    BlockEventsHandler.cpp
    FullySupportedSignalValue.cpp
    SimulationOutput.cpp
    ContinuousAndOde/BasicOdeSolver.cpp
    ContinuousAndOde/EulerForwardStepSolver.cpp
    ContinuousAndOde/EulerBackwardStepSolver.cpp
    ContinuousAndOde/SolverFactory.cpp
    PortsAndSignalValues/InputPort.cpp 
    PortsAndSignalValues/OutputPort.cpp 
    PortsAndSignalValues/Port.cpp 
)

set_target_properties(PySysLinkBaseCore PROPERTIES 
    POSITION_INDEPENDENT_CODE ON
)

add_executable(PySysLinkBase_exe main_dummy.cpp)
set_property(TARGET PySysLinkBase_exe PROPERTY OUTPUT_NAME PySysLinkBase)

# -------------------------------------------------------------------
# YAML-CPP (system)
# -------------------------------------------------------------------
set(YAML_CPP_STATIC_LIBS TRUE)
find_package(yaml-cpp REQUIRED)
target_link_libraries(PySysLinkBaseCore PRIVATE yaml-cpp)

# -------------------------------------------------------------------
# dl
# -------------------------------------------------------------------
target_link_libraries(PySysLinkBaseCore PRIVATE dl)
target_link_libraries(PySysLinkBase_exe PRIVATE dl)

# -------------------------------------------------------------------
# spdlog (system)
# -------------------------------------------------------------------
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
target_link_libraries(PySysLinkBaseCore
    PRIVATE
        spdlog::spdlog
        fmt::fmt
)
# -------------------------------------------------------------------
# Eigen3 (system)
# -------------------------------------------------------------------
find_package(Eigen3 REQUIRED NO_MODULE)
target_link_libraries(PySysLinkBaseCore PRIVATE Eigen3::Eigen)

# -------------------------------------------------------------------
# HDF5 (system)
# -------------------------------------------------------------------
find_package(HDF5 REQUIRED COMPONENTS C CXX)

target_include_directories(PySysLinkBaseCore PRIVATE ${HDF5_INCLUDE_DIRS})
target_link_libraries(PySysLinkBaseCore PRIVATE HDF5::HDF5)
target_link_libraries(PySysLinkBase_exe PRIVATE HDF5::HDF5)

message(STATUS "HDF5_INCLUDE_DIRS: ${HDF5_INCLUDE_DIRS}")
message(STATUS "HDF5_LIBRARIES: ${HDF5_LIBRARIES}")

# -------------------------------------------------------------------
# HighFive (FetchContent)
# -------------------------------------------------------------------
find_package(HighFive REQUIRED)

target_link_libraries(PySysLinkBaseCore PRIVATE HighFive)
target_link_libraries(PySysLinkBase_exe PRIVATE HighFive)

# -------------------------------------------------------------------
# argparse (FetchContent, header-only)
# -------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

target_link_libraries(PySysLinkBase_exe PRIVATE argparse)


# -------------------------------------------------------------------
# Final static library
# -------------------------------------------------------------------
add_library(PySysLinkBase STATIC
  $<TARGET_OBJECTS:PySysLinkBaseCore>
)

target_link_libraries(PySysLinkBase PRIVATE dl)

set_target_properties(PySysLinkBase PROPERTIES 
    POSITION_INDEPENDENT_CODE ON
    VERSION ${PROJECT_VERSION}
    OUTPUT_NAME "${INSTALLED_STATIC_LIB_NAME}"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_include_directories(PySysLinkBase
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# Link core library also into executable
target_link_libraries(PySysLinkBase_exe PRIVATE PySysLinkBaseCore)
