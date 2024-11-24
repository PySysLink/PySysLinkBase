cmake_minimum_required(VERSION 3.11)

# Project name and version
project(PySysLink VERSION 0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# Add subdirectories
add_subdirectory(src)
add_subdirectory(Tests)


# PySysLinkBase CMake Package Configuration
include(CMakePackageConfigHelpers)

# Install PySysLinkBase headers and library
install(TARGETS PySysLinkBase
        EXPORT PySysLinkBaseTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)

# Install headers
install(DIRECTORY src/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h")

# Create a Config file for find_package
install(EXPORT PySysLinkBaseTargets
        FILE PySysLinkBaseTargets.cmake
        NAMESPACE PySysLinkBase::
        DESTINATION lib/cmake/PySysLinkBase)

# Configure a Config file
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/PySysLinkBaseConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/PySysLinkBaseConfig.cmake"
    INSTALL_DESTINATION lib/cmake/PySysLinkBase
)

# Install the Config file
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/PySysLinkBaseConfig.cmake"
        DESTINATION lib/cmake/PySysLinkBase)