# Tests/CMakeLists.txt

# Find GoogleTest (using FetchContent or expect it to be installed on the system)
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/release-1.12.1.zip
)
# For Windows: prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Create an executable for tests
add_executable(PySysLinkTests
    test_main.cpp
    ModelParser_test.cpp
    SimulationModel_test.cpp
    # ... add additional test source files here
)

# Link the main library and gtest
target_link_libraries(PySysLinkTests PRIVATE PySysLinkBase gtest gtest_main spdlog::spdlog Eigen3::Eigen yaml-cpp)

# Enable CTest
include(CTest)
# gtest_add_tests(TARGET PySysLinkTests)
add_test(PySysLinkTests PySysLinkTests)


# Specify the source and destination paths
set(SOURCE_FILE "${CMAKE_SOURCE_DIR}/Tests/dummy_system.yaml")
set(DEST_DIR "${CMAKE_BINARY_DIR}/Tests")

# Copy the file from source to destination
add_custom_command(
    TARGET PySysLinkTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE} ${DEST_DIR}
    COMMENT "Copying dummy_system.yaml to build/Tests"
)