#
# SPDX-FileCopyrightText: Copyright 2024-2025 Arm Limited and/or its affiliates <open-source-office@arm.com>
#
# SPDX-License-Identifier: Apache-2.0
#

# Declare project
project(arm-stt-java-prj
        VERSION     0.0.1
        DESCRIPTION "An STT Java interface"
        LANGUAGES   C CXX ASM)

# Add the Java STT API interface library. This is never going to be built
# but we can use its property to get the interface source used in other
# targets.
add_library(arm-stt-java INTERFACE)

if (${STT_DEP_NAME} STREQUAL "whisper.cpp")
    target_sources(arm-stt-java INTERFACE com/arm/stt/Whisper.java com/arm/stt/WhisperConfig.java)
    add_dependencies(arm-stt-java arm-stt-jni)
else()
    message(FATAL_ERROR "${STT_DEP_NAME} is currently not supported :(")
endif()

