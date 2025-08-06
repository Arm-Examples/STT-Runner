#
# SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its affiliates <open-source-office@arm.com>
#
# SPDX-License-Identifier: Apache-2.0
#

# Makes sure this coverage module is not accidentally processed multiple times
include_guard(GLOBAL)

# Brings in the CMake module shipped with CMake itself, called CMakeDependentOption.cmake
include(CMakeDependentOption)

# It works like a Gate that only allow the option to be enabled when using a Debug build.
# In non-Debug (e.g., Release) builds, the option is hidden and forced OFF, so -DENABLE_COVERAGE=ON won't have any effect
cmake_dependent_option(
  ENABLE_COVERAGE
  "Enable GCC code coverage instrumentation"
  OFF
  [[CMAKE_BUILD_TYPE STREQUAL "Debug"]]
  OFF
)

# ENABLE_COVERAGE will only exist in Debug builds
if(ENABLE_COVERAGE)
    message(STATUS "GCC code coverage instrumentation enabled")

    # Enforce Debug build type
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(FATAL_ERROR "Code coverage is only supported in Debug builds. Please use -DCMAKE_BUILD_TYPE=Debug")
    endif()

    # Only apply if using GCC (both C and C++)
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage -O0 -g -fdebug-prefix-map=${CMAKE_SOURCE_DIR}=.")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -O0 -g -fdebug-prefix-map=${CMAKE_SOURCE_DIR}=.")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
    else()
        message(FATAL_ERROR "Coverage requires GCC for both C and C++ compilers")
    endif()
endif()
