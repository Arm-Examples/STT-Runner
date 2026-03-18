#
# SPDX-FileCopyrightText: Copyright 2025-2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
#
# SPDX-License-Identifier: Apache-2.0
#

include_guard(GLOBAL)
set(STT_DEP_NAME "whisper.cpp" CACHE STRING
    "Dependency name to configure the project for")

set(DOWNLOADS_DIR        ${CMAKE_CURRENT_SOURCE_DIR}/resources_downloaded
CACHE STRING
"Directory where required resources are downloaded into")

set(DOWNLOADS_LOCK_TIMEOUT 600
    CACHE STRING
    "Timeout in seconds for lock to hold off concurrent CMake configurations
    trying to download resources to the same directory.")

option(BUILD_EXECUTABLE  "Build stand-alone executables"     OFF)
option(BUILD_UNIT_TESTS  "Build unit tests"                  ON)
option(BUILD_JNI_LIB     "Build JNI lib"                     ON)
option(BUILD_SHARED_LIBS "Build shared libraries"            OFF)
option(ENABLE_STREAMLINE "Enable Arm Streamline annotations" OFF)
