# Add test executable
add_executable(PySysLinkExamples test_main.cpp)
add_executable(ExampleContinuous test_continuous.cpp)
add_executable(ExampleContinuousImplicit test_continuous_implicit.cpp)
add_executable(ExampleMultirate test_multirate.cpp)
add_executable(ExampleStep test_step_continuous.cpp)

# Include directories
target_include_directories(PySysLinkExamples PRIVATE 
    ${CMAKE_SOURCE_DIR}/src # Source headers
    ${CMAKE_SOURCE_DIR}/Examples # Test-specific headers
)

find_package(HDF5 REQUIRED)
include_directories(${HDF5_INCLUDE_DIRS})

find_package(Eigen3 REQUIRED NO_MODULE)
find_package(fmt REQUIRED)


# Link the main project (if necessary)
target_link_libraries(PySysLinkExamples PRIVATE PySysLinkBase spdlog Eigen3::Eigen yaml-cpp HighFive HDF5::HDF5 ${HDF5_LIBRARIES} fmt::fmt)
target_link_libraries(ExampleContinuous PRIVATE PySysLinkBase spdlog Eigen3::Eigen yaml-cpp HighFive HDF5::HDF5 ${HDF5_LIBRARIES} fmt::fmt)
target_link_libraries(ExampleContinuousImplicit PRIVATE PySysLinkBase spdlog Eigen3::Eigen yaml-cpp HighFive HDF5::HDF5 ${HDF5_LIBRARIES} fmt::fmt)
target_link_libraries(ExampleMultirate PRIVATE PySysLinkBase spdlog Eigen3::Eigen yaml-cpp HighFive HDF5::HDF5 ${HDF5_LIBRARIES} fmt::fmt)
target_link_libraries(ExampleStep PRIVATE PySysLinkBase spdlog Eigen3::Eigen yaml-cpp HighFive HDF5::HDF5 ${HDF5_LIBRARIES} fmt::fmt)


set_target_properties(PySysLinkExamples PROPERTIES
    BUILD_RPATH "$ORIGIN/../src" # Ensure runtime lookup prioritizes local build
    INSTALL_RPATH "$ORIGIN/../src"
)

# Add a test for CTest
enable_testing()
add_test(NAME PySysLinkExamples COMMAND PySysLinkExamples)

# Specify the source and destination paths
set(SOURCE_FILE "${CMAKE_SOURCE_DIR}/Examples/system1.yaml")
set(DEST_DIR "${CMAKE_BINARY_DIR}/Examples")

# Copy the file from source to destination
add_custom_command(
    TARGET PySysLinkExamples POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE} ${DEST_DIR}
    COMMENT "Copying system1.yaml to build/Examples"
)