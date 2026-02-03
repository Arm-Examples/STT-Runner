#
# SPDX-FileCopyrightText: Copyright 2025-2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
#
# SPDX-License-Identifier: Apache-2.0
#

# Declare project
project(whisper-cpp-wrapper
        VERSION     0.0.1
        DESCRIPTION "whisper.cpp wrapper interface implementation"
        LANGUAGES   C CXX ASM)

include(FetchContent)


set(WHISPER_SRC_DIR "${CMAKE_BINARY_DIR}/whisper.cpp"
        CACHE PATH
        "Path where WHISPER (dependency) repo should be cloned into")

set(WHISPER_GIT_URL "https://github.com/ggerganov/whisper.cpp.git"
        CACHE STRING
        "Git URL for Whisper (dependency) repo")

set(WHISPER_GIT_TAG "v1.8.3"
        CACHE STRING
        "Git commit TAG for whisper.cpp repo")

set(WHISPER_BUILD_EXAMPLES ${BUILD_EXECUTABLE} CACHE BOOL "Build Whisper.cpp examples") 

if (NOT EXISTS ${WHISPER_SRC_DIR})
    message(STATUS "Populating and adding ${WHISPER_SRC_DIR}")
endif()

# Identify system processor for Arm
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" lower_system_processor)
if (lower_system_processor MATCHES "^(aarch64|arm64)$")
    set(ARM_SYSTEM_PROCESSOR ON BOOL)
endif()

# If the user has NOT explicitly set GGML_CPU_KLEIDIAI
if(DEFINED USE_KLEIDIAI AND NOT DEFINED GGML_CPU_KLEIDIAI)
    set(GGML_CPU_KLEIDIAI ${USE_KLEIDIAI})
endif()
if (NOT DEFINED GGML_CPU_KLEIDIAI)
    # if we are on arm64/aarch64, then default KleidiAI to ON.
    if (ARM_SYSTEM_PROCESSOR)
        set(GGML_CPU_KLEIDIAI ON CACHE BOOL
            "Enable KleidiAI by default on ${CMAKE_SYSTEM_PROCESSOR}")
        message(STATUS "KleidiAI enabled by default")
    # if we are NOT on arm64/aarch64, then default KleidiAI to OFF.
    else()
        set(GGML_CPU_KLEIDIAI OFF CACHE BOOL
            "Disable KleidiAI by default on ${CMAKE_SYSTEM_PROCESSOR}")
        message(STATUS "KleidiAI disabled by default")
    endif()
else ()
    message(STATUS "KleidiAI: ${GGML_CPU_KLEIDIAI}")
endif()

# Avoid enabling KleidiAI for native builds
if(GGML_CPU_KLEIDIAI AND NOT ARM_SYSTEM_PROCESSOR)
    message(FATAL_ERROR "KleidiAI enabled, but not supported on ${CMAKE_SYSTEM_PROCESSOR}")
endif()

# -----------------------------------------
# NOTE: Current work flow for i8mm implementation:
# If user provides any -march flag, check if compiler supports it.
# -----------------------------------------

# Include functions for flag checking
include(check-flag)

# Define the list of supported languages.
set(SUPPORTED_LANGUAGES C CXX)

# Check and set C,CXX flags
foreach(LANG ${SUPPORTED_LANGUAGES})
    # Check for -march flag in C flags.
    string(REGEX MATCH "-march=[^ ]+" USER_${LANG}_MARCH_FLAG "${CMAKE_${LANG}_FLAGS}")
    if (USER_${LANG}_MARCH_FLAG)
        message(STATUS "User provided -march flag in ${LANG} flags: ${USER_${LANG}_MARCH_FLAG}")
        # Check if the compiler supports the provided C,CXX -march flag
        check_compiler_support("${LANG}" "${USER_${LANG}_MARCH_FLAG}")
    endif()
endforeach()

# Fetch the dependency Git repo here
FetchContent_Declare(whisper-cpp
    GIT_REPOSITORY  ${WHISPER_GIT_URL}
    GIT_TAG         ${WHISPER_GIT_TAG}
    GIT_SHALLOW     1 # We only need shallow clone with `--depth=1`
    SOURCE_DIR      ${WHISPER_SRC_DIR}
    )

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
FetchContent_MakeAvailable(whisper-cpp)

if (NOT TARGET arm-stt-cpp)
    add_library(arm-stt-cpp INTERFACE)
    target_include_directories(arm-stt-cpp INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}/../include)
endif()

# Add the current
target_include_directories(arm-stt-cpp INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

# List all libraries that we need to depend on here:
target_link_libraries(arm-stt-cpp INTERFACE
    ggml
    ggml-base
    whisper)

if(BUILD_JNI_LIB)
    # Make sure JNI include directories have been set.
    include(find-jni)

    add_library(arm-stt-jni SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/jni/Whisper.cpp)

    target_link_libraries(arm-stt-jni PUBLIC arm-stt-cpp)
    target_include_directories(arm-stt-jni PUBLIC
        ${JNI_INCLUDE_DIRS} # Populated by FindJNI CMake module
    )
endif()
