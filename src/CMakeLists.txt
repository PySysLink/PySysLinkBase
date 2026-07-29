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
    PortsAndSignalValues/PortTypeMetadata.cpp
)

set_target_properties(PySysLinkBaseCore PROPERTIES 
    POSITION_INDEPENDENT_CODE ON
)

add_executable(PySysLinkBase_exe main.cpp)
set_property(TARGET PySysLinkBase_exe PROPERTY OUTPUT_NAME PySysLinkBase)

# -------------------------------------------------------------------
# YAML-CPP (system)
# -------------------------------------------------------------------
find_package(yaml-cpp REQUIRED)

add_library(yaml-cpp-static STATIC IMPORTED)

set_target_properties(yaml-cpp-static PROPERTIES
    IMPORTED_LOCATION "/usr/lib64/libyaml-cpp.a"
    INTERFACE_INCLUDE_DIRECTORIES "/usr/include"
)

target_link_libraries(PySysLinkBaseCore PRIVATE yaml-cpp-static)

# -------------------------------------------------------------------
# dl
# -------------------------------------------------------------------
target_link_libraries(PySysLinkBaseCore PRIVATE dl)
target_link_libraries(PySysLinkBase_exe PRIVATE dl)

# -------------------------------------------------------------------
# spdlog (system)
# -------------------------------------------------------------------

include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 11.2.0
)

set(FMT_TEST OFF CACHE BOOL "" FORCE)
set(FMT_DOC OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(fmt)

find_package(spdlog REQUIRED)

target_link_libraries(PySysLinkBaseCore
    PRIVATE
        spdlog::spdlog
        fmt::fmt
)
# -------------------------------------------------------------------
# Eigen3 (system)
# -------------------------------------------------------------------
find_package(Eigen3 REQUIRED NO_MODULE)
target_link_libraries(PySysLinkBaseCore PUBLIC Eigen3::Eigen)
target_link_libraries(PySysLinkBase_exe PUBLIC Eigen3::Eigen)

# -------------------------------------------------------------------
# HDF5 (system)
# -------------------------------------------------------------------
find_package(ZLIB REQUIRED)

find_library(SZ_LIBRARY
    NAMES sz
)

message(STATUS "SZ_LIBRARY = ${SZ_LIBRARY}")

set(HDF5_USE_STATIC_LIBRARIES TRUE)
find_package(HDF5 REQUIRED COMPONENTS C CXX)

target_include_directories(PySysLinkBaseCore PRIVATE ${HDF5_INCLUDE_DIRS})
target_link_libraries(PySysLinkBaseCore PRIVATE
    HDF5::HDF5
    ZLIB::ZLIB
    ${SZ_LIBRARY}
)

target_link_libraries(PySysLinkBase_exe PRIVATE
    HDF5::HDF5
    ZLIB::ZLIB
    ${SZ_LIBRARY}
)

message(STATUS "HDF5_INCLUDE_DIRS: ${HDF5_INCLUDE_DIRS}")
message(STATUS "HDF5_LIBRARIES: ${HDF5_LIBRARIES}")

# -------------------------------------------------------------------
# HighFive
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

target_link_libraries(PySysLinkBase
    PUBLIC
        Eigen3::Eigen
        yaml-cpp
        HighFive
        HDF5::HDF5
    PRIVATE
        dl
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
target_sources(PySysLinkBase_exe PRIVATE $<TARGET_OBJECTS:PySysLinkBaseCore>)
target_link_libraries(PySysLinkBase_exe PRIVATE PySysLinkBaseCore)
