#
# SPDX-FileCopyrightText: Copyright 2025-2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
#
# SPDX-License-Identifier: Apache-2.0
#

include_guard(GLOBAL)
include(configuration-options)
include(python-deps)

set(STT_NUMBA_CACHE_DIR "${CMAKE_BINARY_DIR}/.stt-numba-cache")
file(MAKE_DIRECTORY "${STT_NUMBA_CACHE_DIR}")
set(ENV{NUMBA_CACHE_DIR} "${STT_NUMBA_CACHE_DIR}")

set(STT_LIBROSA_PIP_PACKAGE "librosa")
set(STT_LIBROSA_PIP_VERSION "0.9.2")
set(STT_LIBROSA_PIP_SPEC "${STT_LIBROSA_PIP_PACKAGE}==${STT_LIBROSA_PIP_VERSION}")
stt_ensure_python_dependency("librosa" "${STT_LIBROSA_PIP_SPEC}")

message(STATUS "Converting audio from ${WHISPER_SRC_DIR}/samples/jfk.wav "
        "to ${CMAKE_CURRENT_SOURCE_DIR}/resources/audioData.csv")

set(STT_AUDIO_DATA_CSV "${CMAKE_CURRENT_SOURCE_DIR}/resources/audioData.csv")

execute_process(
    COMMAND ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/py/convert_wav_to_csv.py
        --wav_file_path
        ${WHISPER_SRC_DIR}/samples/jfk.wav
        --output_file_path
        ${STT_AUDIO_DATA_CSV}
        --sample_rate
        16000
    RESULT_VARIABLE return_code)

if (NOT return_code STREQUAL "0")
    message(FATAL_ERROR "Failed to convert test wav file. Error code ${return_code}")
endif ()

if (NOT EXISTS "${STT_AUDIO_DATA_CSV}")
    message(FATAL_ERROR "Expected audio data CSV was not generated: ${STT_AUDIO_DATA_CSV}")
endif()

file(SIZE "${STT_AUDIO_DATA_CSV}" stt_audio_data_csv_size)
message(STATUS "Generated audio CSV: ${STT_AUDIO_DATA_CSV} (${stt_audio_data_csv_size} bytes)")
if (stt_audio_data_csv_size EQUAL 0)
    message(FATAL_ERROR "Generated audio data CSV is empty: ${STT_AUDIO_DATA_CSV}")
endif()
