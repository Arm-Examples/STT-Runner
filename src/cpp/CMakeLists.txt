#
# SPDX-FileCopyrightText: Copyright 2025-2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
#
# SPDX-License-Identifier: Apache-2.0
#

# Declare project
project(stt-cpp-prj
        VERSION     0.0.1
        DESCRIPTION "An STT CPP interface"
        LANGUAGES   C CXX ASM)
# Add log
add_subdirectory(log)

# Add the STT API interface library
# NOTE: This is an interface library right now because our wrapper is
#       only a header file. If we need to add source files later, this
#       would change to a SHARED or STATIC library.
add_library(stt-cpp INTERFACE)
target_include_directories(stt-cpp INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(stt-cpp INTERFACE stt-log)
if(ENABLE_STREAMLINE)
    include(streamline)
    stt_enable_streamline(stt-cpp)
endif()

if (${STT_DEP_NAME} STREQUAL "whisper.cpp")
    add_subdirectory(whisper_cpp)
else()
    message(FATAL_ERROR "${STT_DEP_NAME} is currently not supported :(")
endif()

# Add benchmark targets only when explicitly enabled.
if(BUILD_BENCHMARK)
    add_subdirectory(benchmark)
endif()
