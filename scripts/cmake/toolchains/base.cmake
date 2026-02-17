#
# SPDX-FileCopyrightText: Copyright 2025-2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
#
# SPDX-License-Identifier: Apache-2.0
#

include_guard(GLOBAL)

if (NOT CMAKE_CROSSCOMPILING)
    set(CMAKE_TRY_COMPILE_PLATFORM_VARIABLES CPU_ARCH TARGET_PLATFORM)
endif()

# BUILD_DEBUG defaults to OFF unless provided by CLI/presets.
if(NOT DEFINED BUILD_DEBUG OR NOT BUILD_DEBUG)
    set(BUILD_DEBUG OFF CACHE BOOL "Enable debug logging defaults")
    set (CMAKE_BUILD_TYPE "Release")
else()
    # Ensure it's shown as a BOOL in cmake-gui/ccmake, keep user's value.
    set_property(CACHE BUILD_DEBUG PROPERTY TYPE BOOL)
    set_property(CACHE BUILD_DEBUG PROPERTY HELPSTRING "Enable debug logging defaults")
    set (CMAKE_BUILD_TYPE "Debug")
endif()

message(STATUS "BUILD_DEBUG = ${BUILD_DEBUG}")
message(STATUS "BUILD_JNI_LIB     = ${BUILD_JNI_LIB}")
message(STATUS "CMAKE_BUILD_TYPE  = ${CMAKE_BUILD_TYPE}")
