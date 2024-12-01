cmake_minimum_required(VERSION 3.11)

# Project name and version
project(PySysLink VERSION 0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# Add subdirectories
add_subdirectory(src)
add_subdirectory(Tests)


set(PY_SYS_LINK_BASE_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/include")
set(PY_SYS_LINK_BASE_LIBRARIES "${CMAKE_INSTALL_PREFIX}/lib/libPySysLinkBase.a")

message("CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

# Define installation rules for the project
install(DIRECTORY src/PySysLinkBase/                # Install headers
    DESTINATION include/PySysLinkBase
    FILES_MATCHING PATTERN "*.h"
)

install(TARGETS PySysLinkBase
    EXPORT PySysLinkBaseTargets # Associate this target with the export
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)


include(CMakePackageConfigHelpers)

# Generate the package configuration file for PySysLinkBase
install(EXPORT PySysLinkBaseTargets
    FILE PySysLinkBaseTargets.cmake
    NAMESPACE PySysLinkBase::
    DESTINATION lib/cmake/PySysLinkBase
)

# Set the include directories and libraries for the package
set(PACKAGE_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/include")
set(PACKAGE_LIBRARIES "${CMAKE_INSTALL_PREFIX}/lib")


# Create and install the config file
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/PySysLinkBaseConfig.cmake.in
    ${CMAKE_BINARY_DIR}/PySysLinkBaseConfig.cmake
    INSTALL_DESTINATION lib/cmake/PySysLinkBase
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/PySysLinkBaseConfig.cmake
    DESTINATION lib/cmake/PySysLinkBase
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/PySysLinkBase/ DESTINATION include)

# Ensure the libraries are installed correctly
install(TARGETS PySysLinkBase
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib)
