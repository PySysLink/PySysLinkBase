# Add test executable
add_executable(PySysLinkTests test_main.cpp)
add_executable(Continuous test_continuous.cpp)
add_executable(ContinuousImplicit test_continuous_implicit.cpp)
add_executable(Multirate test_multirate.cpp)
add_executable(Step test_step_continuous.cpp)

# Include directories
target_include_directories(PySysLinkTests PRIVATE 
    ${CMAKE_SOURCE_DIR}/src # Source headers
    ${CMAKE_SOURCE_DIR}/Tests # Test-specific headers
)

# Link the main project (if necessary)
target_link_libraries(PySysLinkTests PRIVATE PySysLinkBase)
target_link_libraries(Continuous PRIVATE PySysLinkBase)
target_link_libraries(ContinuousImplicit PRIVATE PySysLinkBase)
target_link_libraries(Multirate PRIVATE PySysLinkBase)
target_link_libraries(Step PRIVATE PySysLinkBase)

# Add test framework (if using one, like GoogleTest)
# Example: Linking with GoogleTest
# find_package(GTest REQUIRED)
# target_link_libraries(MyCppTests PRIVATE GTest::GTest GTest::Main)

set_target_properties(PySysLinkTests PROPERTIES
    BUILD_RPATH "$ORIGIN/../src" # Ensure runtime lookup prioritizes local build
    INSTALL_RPATH "$ORIGIN/../src"
)

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