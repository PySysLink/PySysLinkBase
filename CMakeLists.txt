cmake_minimum_required(VERSION 3.11)

# Project name and version
project(PySysLink VERSION 0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# Add subdirectories
add_subdirectory(src)
add_subdirectory(Tests)
