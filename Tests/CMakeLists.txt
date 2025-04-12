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

# Create an executable for tests
add_executable(PySysLinkTests
    test_main.cpp
    ModelParser_test.cpp
    SimulationModel_test.cpp
    # ... add additional test source files here
)

# Link the main library and gtest
target_link_libraries(PySysLinkTests PRIVATE PySysLinkBase gtest gtest_main)

# Enable CTest
include(CTest)
add_test(NAME PySysLinkTests COMMAND PySysLinkTests)