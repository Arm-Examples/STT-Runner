#
# SPDX-FileCopyrightText: Copyright 2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
#
# SPDX-License-Identifier: Apache-2.0
#
include_guard(GLOBAL)

function(arm_stt_enable_streamline target_name)
    message(STATUS "STREAMLINE ENABLED for target: ${target_name}")
    include(FetchContent)

    # Fetch Arm Gator sources for Streamline annotations.
    set(GATOR_SRC_DIR "${CMAKE_BINARY_DIR}/gator"
        CACHE PATH "Streamline annotate source dir")

    set(GATOR_GIT_URL "https://github.com/ARM-software/gator.git"
        CACHE STRING "Git URL for Gator repo")

    # Gator v9.7.2 compatible with Arm Performance Studio 2025.6.
    set(GATOR_GIT_TAG "f0774012f36dbdb543e082d3e14ca9db20d0432d"
        CACHE STRING "Git tag / commit SHA for Gator repo")

    FetchContent_Declare(streamline_annotate_src
        GIT_REPOSITORY ${GATOR_GIT_URL}
        GIT_TAG        ${GATOR_GIT_TAG}
        GIT_SHALLOW    1
        SOURCE_DIR     ${GATOR_SRC_DIR}
        EXCLUDE_FROM_ALL
    )

    FetchContent_Populate(streamline_annotate_src)

    if(NOT TARGET stt_streamline_annotate)
        add_library(stt_streamline_annotate STATIC
            ${GATOR_SRC_DIR}/annotate/streamline_annotate.c
        )

        target_include_directories(stt_streamline_annotate PUBLIC
            ${GATOR_SRC_DIR}/annotate
        )

        set_property(TARGET stt_streamline_annotate
            PROPERTY POSITION_INDEPENDENT_CODE ON)
    endif()

    if(NOT TARGET arm_stt_streamline)
        add_library(arm_stt_streamline INTERFACE)

        target_link_libraries(arm_stt_streamline INTERFACE
            stt_streamline_annotate
        )

        target_compile_definitions(arm_stt_streamline INTERFACE
            ENABLE_STREAMLINE
        )

        target_compile_options(arm_stt_streamline INTERFACE
            -g
            -fno-omit-frame-pointer
            -fno-inline
        )

        target_include_directories(arm_stt_streamline INTERFACE
            ${PROJECT_SOURCE_DIR}/src/cpp
        )
    endif()

    get_target_property(target_type ${target_name} TYPE)
    if(target_type STREQUAL "INTERFACE_LIBRARY")
        target_link_libraries(${target_name} INTERFACE arm_stt_streamline)
        return()
    endif()

    target_link_libraries(${target_name} PUBLIC arm_stt_streamline)

    set_property(TARGET ${target_name}
        PROPERTY POSITION_INDEPENDENT_CODE ON)
endfunction()
