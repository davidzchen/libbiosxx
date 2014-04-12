cmake_minimum_required(VERSION 2.6)
project(libbiosxx)

set(libbiosxx_VERSION_MAJOR 0)
set(libbiosxx_VERSION_MINOR 1)
set(libbiosxx_VERSION_PATCH 0)

# Set the CMake module path to /cmake.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

# Set include directory.
include_directories("${PROJECT_SOURCE_DIR}")

# Set CXXFLAGS.
set(CMAKE_CXX_FLAGS "-Werror -Wall")

# Generate CTest input files.
enable_testing()

# Find packages.
find_package(GSL REQUIRED)
include_directories(SYSTEM, ${GSL_INCLUDE_DIRS})
set(LIBS ${LIBS} ${GSL_LIBRARIES})
message(STATUS "GSL include dir: " ${GSL_INCLUDE_DIRS})
message(STATUS "GSL libraries: " ${GSL_LIBRARIES})

# Add the bios subdirectory.
add_subdirectory(bios)
