# Define a library
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

add_executable(PySysLinkBase_exe main.cpp)
set_property(TARGET PySysLinkBase_exe PROPERTY OUTPUT_NAME PySysLinkBase)


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

target_link_libraries(PySysLinkBaseCore PRIVATE yaml-cpp) # The library or executable that require yaml-cpp library


FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.15.0
)

FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
  Eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG master
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE)
# note: To disable eigen tests,
# you should put this code in a add_subdirectory to avoid to change
# BUILD_TESTING for your own project too since variables are directory
# scoped
set(BUILD_TESTING OFF)
set(EIGEN_BUILD_TESTING OFF)
set(EIGEN_MPL2_ONLY ON)
set(EIGEN_BUILD_PKGCONFIG OFF)
set(EIGEN_BUILD_DOC OFF)
FetchContent_MakeAvailable(Eigen)
 
list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "fetched")

target_link_libraries (PySysLinkBaseCore PRIVATE Eigen3::Eigen)

find_package(HDF5 REQUIRED COMPONENTS C CXX)

FetchContent_Declare(
  highfive
  GIT_REPOSITORY https://github.com/highfive-devs/highfive.git
  GIT_TAG        v2.10.1           
  GIT_SHALLOW    ON
)

FetchContent_GetProperties(highfive)
FetchContent_MakeAvailable(highfive)
target_include_directories(PySysLinkBaseCore PRIVATE ${HDF5_INCLUDE_DIRS})
include_directories(BEFORE SYSTEM ${highfive_SOURCE_DIR} ${highfive_BINARY_DIR}/include)
include_directories(BEFORE SYSTEM ${HDF5_INCLUDE_DIRS})
target_link_libraries(PySysLinkBaseCore PRIVATE HighFive)
target_link_libraries(PySysLinkBaseCore PRIVATE HDF5::HDF5)
target_link_libraries(PySysLinkBaseCore PRIVATE ${HDF5_LIBRARIES})
target_link_libraries(PySysLinkBase_exe PRIVATE HighFive)
target_link_libraries(PySysLinkBase_exe PRIVATE HDF5::HDF5)
target_link_libraries(PySysLinkBase_exe PRIVATE ${HDF5_LIBRARIES})

message(STATUS "HDF5_INCLUDE_DIRS: ${HDF5_INCLUDE_DIRS}")
message(STATUS "HDF5_LIBRARIES: ${HDF5_LIBRARIES}")

FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

target_link_libraries(PySysLinkBase_exe PRIVATE argparse)
target_link_libraries(PySysLinkBase_exe PRIVATE PySysLinkBaseCore)


include(GNUInstallDirs)



target_link_libraries(PySysLinkBaseCore PRIVATE spdlog::spdlog)

add_library(PySysLinkBase STATIC
  $<TARGET_OBJECTS:PySysLinkBaseCore>
)

set_target_properties(PySysLinkBase PROPERTIES 
    POSITION_INDEPENDENT_CODE ON
    VERSION ${PROJECT_VERSION}
    IMPORTED_LOCATION "${CMAKE_INSTALL_PREFIX}${INSTALLED_STATIC_LIB_NAME}.a"
    OUTPUT_NAME "${INSTALLED_STATIC_LIB_NAME}"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)


# Include directories for header files
target_include_directories(PySysLinkBase
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>  # Include path for the build tree
        $<INSTALL_INTERFACE:include>                   # Include path for the installed tree
)