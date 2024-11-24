# Add test executable
add_executable(PySysLinkTests test_main.cpp)


# Include directories
target_include_directories(PySysLinkTests PRIVATE ${CMAKE_SOURCE_DIR}/Tests)
target_include_directories(PySysLinkTests PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Link the main project (if necessary)
target_link_libraries(PySysLinkTests PRIVATE PySysLinkBase)
target_link_libraries(PySysLinkTests PRIVATE BasicCppSupport)

# Add test framework (if using one, like GoogleTest)
# Example: Linking with GoogleTest
# find_package(GTest REQUIRED)
# target_link_libraries(MyCppTests PRIVATE GTest::GTest GTest::Main)

# Add a test for CTest
enable_testing()
add_test(NAME PySysLinkTests COMMAND PySysLinkTests)

# Specify the source and destination paths
set(SOURCE_FILE "${CMAKE_SOURCE_DIR}/Tests/system1.yaml")
set(DEST_DIR "${CMAKE_BINARY_DIR}/Tests")

# Copy the file from source to destination
add_custom_command(
    TARGET PySysLinkTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE} ${DEST_DIR}
    COMMENT "Copying system1.yaml to build/Tests"
)