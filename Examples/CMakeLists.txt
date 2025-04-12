# Add test executable
add_executable(PySysLinkExamples test_main.cpp)
add_executable(Continuous test_continuous.cpp)
add_executable(ContinuousImplicit test_continuous_implicit.cpp)
add_executable(Multirate test_multirate.cpp)
add_executable(Step test_step_continuous.cpp)

# Include directories
target_include_directories(PySysLinkExamples PRIVATE 
    ${CMAKE_SOURCE_DIR}/src # Source headers
    ${CMAKE_SOURCE_DIR}/Examples # Test-specific headers
)

# Link the main project (if necessary)
target_link_libraries(PySysLinkExamples PRIVATE PySysLinkBase)
target_link_libraries(Continuous PRIVATE PySysLinkBase)
target_link_libraries(ContinuousImplicit PRIVATE PySysLinkBase)
target_link_libraries(Multirate PRIVATE PySysLinkBase)
target_link_libraries(Step PRIVATE PySysLinkBase)


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