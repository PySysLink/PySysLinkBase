# Add test executable
add_executable(PySysLinkTests test_main.cpp)

# Include directories
target_include_directories(PySysLinkTests PRIVATE ${CMAKE_SOURCE_DIR}/Tests)
target_include_directories(PySysLinkTests PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Link the main project (if necessary)
target_link_libraries(PySysLinkTests PRIVATE PySysLinkBase)

# Add test framework (if using one, like GoogleTest)
# Example: Linking with GoogleTest
# find_package(GTest REQUIRED)
# target_link_libraries(MyCppTests PRIVATE GTest::GTest GTest::Main)

# Add a test for CTest
enable_testing()
add_test(NAME PySysLinkTests COMMAND PySysLinkTests)