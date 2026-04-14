#
# SPDX-FileCopyrightText: Copyright 2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
#
# SPDX-License-Identifier: Apache-2.0
#

project(stt-benchmark
        VERSION 0.0.1
        DESCRIPTION "STT Benchmark"
        LANGUAGES CXX)

add_library(stt-benchmark STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/SttBenchmark.cpp)

target_include_directories(stt-benchmark PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_features(stt-benchmark PUBLIC cxx_std_17)

target_link_libraries(stt-benchmark PUBLIC
    stt-cpp)

if(BUILD_BENCHMARK)
    add_executable(stt-bench
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
    target_link_libraries(stt-bench PRIVATE stt-benchmark)
    target_compile_features(stt-bench PRIVATE cxx_std_17)
endif()
