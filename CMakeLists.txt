cmake_minimum_required(VERSION 2.6)
project(libbiosxx)

set(libbiosxx_VERSION_MAJOR 0)
set(libbiosxx_VERSION_MINOR 1)
set(libbiosxx_VERSION_PATCH 0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

# Set include directory.
include_directories("${PROJECT_SOURCE_DIR}")

# Set CXXFLAGS.
set(CMAKE_CXX_FLAGS "-Werror -Wall")

# Generate CTest input files.
enable_testing()

find_package(GSL REQUIRED)

# Add the bios subdirectory.
add_subdirectory(bios)
