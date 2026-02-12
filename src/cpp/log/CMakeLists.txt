#
# SPDX-FileCopyrightText: Copyright 2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
#
# SPDX-License-Identifier: Apache-2.0
#
project(arm-stt-log
        VERSION 0.0.1
        DESCRIPTION "STT log"
        LANGUAGES CXX)

# Add the STT log library
add_library(arm-stt-log STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Logger.cpp)

set_target_properties(arm-stt-log PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(arm-stt-log PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

set(STT_LOG_LEVEL "" CACHE STRING "choose log level (ERROR,WARN,INFO,DEBUG,VERBOSE)")
set(VALID_LEVELS "ERROR;WARN;INFO;DEBUG;VERBOSE")
set_property(CACHE STT_LOG_LEVEL PROPERTY STRINGS ${VALID_LEVELS})

# make uniform build type string
string(TOUPPER "${CMAKE_BUILD_TYPE}" BUILD_TYPE)

# If user provided a LOG_LEVEL, validate and use it
if(NOT "${STT_LOG_LEVEL}" STREQUAL "")
    list(FIND VALID_LEVELS "${STT_LOG_LEVEL}" idx)
    if(idx EQUAL -1)
        message(FATAL_ERROR "Invalid STT_LOG_LEVEL='${STT_LOG_LEVEL}'. Valid values: ${VALID_LEVELS}")
    endif()
    # Map string level to number
    set(ACTIVE_LOG_LEVEL ${idx})
    message(STATUS "Using user-provided STT_LOG_LEVEL='${STT_LOG_LEVEL}' which is ${ACTIVE_LOG_LEVEL}")
    # Map build type to log level
else()
    if(BUILD_TYPE STREQUAL "DEBUG")
        list(FIND VALID_LEVELS "DEBUG" idx)
    elseif(BUILD_TYPE STREQUAL "RELWITHDEBINFO")
        list(FIND VALID_LEVELS "INFO" idx)
    elseif(BUILD_TYPE STREQUAL "RELEASE")
        list(FIND VALID_LEVELS "WARN" idx)
    elseif(BUILD_TYPE STREQUAL "MINSIZEREL")
        list(FIND VALID_LEVELS "ERROR" idx)
    else()
        message(WARNING "Unknown CMAKE_BUILD_TYPE='${CMAKE_BUILD_TYPE}', defaulting log level to ERROR")
        list(FIND VALID_LEVELS "ERROR" idx)
    endif()
    set(ACTIVE_LOG_LEVEL ${idx})
    list(GET VALID_LEVELS ${ACTIVE_LOG_LEVEL} DEDUCED_LEVEL)
    message(STATUS "Using deduced STT_LOG_LEVEL='${DEDUCED_LEVEL}' (${ACTIVE_LOG_LEVEL})")
endif()

target_compile_definitions(arm-stt-log PUBLIC ACTIVE_LOG_LEVEL=${ACTIVE_LOG_LEVEL})

if(ANDROID)
    find_library(log-lib log)
    if(log-lib)
        target_link_libraries(arm-stt-log PUBLIC ${log-lib})
    else()
        message(WARNING "Android liblog not found — __android_log_print may be undefined")
    endif()
endif()
