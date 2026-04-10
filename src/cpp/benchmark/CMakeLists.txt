#
# SPDX-FileCopyrightText: Copyright 2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
#
# SPDX-License-Identifier: Apache-2.0
#

project(arm-stt-benchmark
        VERSION 0.0.1
        DESCRIPTION "STT Benchmark"
        LANGUAGES CXX)

add_library(arm-stt-benchmark STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/SttBenchmark.cpp)

target_include_directories(arm-stt-benchmark PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_features(arm-stt-benchmark PUBLIC cxx_std_17)

target_link_libraries(arm-stt-benchmark PUBLIC
    arm-stt-cpp)

if(BUILD_BENCHMARK)
    add_executable(arm-stt-bench
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
    target_link_libraries(arm-stt-bench PRIVATE arm-stt-benchmark)
    target_compile_features(arm-stt-bench PRIVATE cxx_std_17)
endif()
