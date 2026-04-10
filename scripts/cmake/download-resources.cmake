#
# SPDX-FileCopyrightText: Copyright 2025-2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
#
# SPDX-License-Identifier: Apache-2.0
#

include_guard(GLOBAL)
include(configuration-options)
include(python-deps)

set(STT_HF_HUB_PIP_PACKAGE "huggingface_hub")
set(STT_HF_HUB_PIP_CONSTRAINT ">=0.20.0")
set(STT_HF_HUB_PIP_SPEC "${STT_HF_HUB_PIP_PACKAGE}${STT_HF_HUB_PIP_CONSTRAINT}")
stt_ensure_python_dependency("huggingface_hub" "${STT_HF_HUB_PIP_SPEC}")

# If the downloads directory doesn't exist, create one
if (NOT EXISTS ${DOWNLOADS_DIR})
    file(MAKE_DIRECTORY ${DOWNLOADS_DIR})
endif()

# Create a lock so other instances of CMake configuration processes hold
# here until the lock is available.
message(STATUS "Waiting to lock resource ${DOWNLOADS_DIR} "
               "Timeout: ${DOWNLOADS_LOCK_TIMEOUT} seconds.")
file(LOCK ${DOWNLOADS_DIR}
    DIRECTORY
    GUARD PROCESS
    RESULT_VARIABLE lock_return_code
    TIMEOUT ${DOWNLOADS_LOCK_TIMEOUT})

if (NOT ${lock_return_code} STREQUAL 0)
    message(FATAL_ERROR "Failed to acquire lock for dir ${DOWNLOADS_DIR}")
endif()
message(STATUS "${DOWNLOADS_DIR} locked; running downloads script...")

execute_process(
    COMMAND ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/scripts/py/download_resources.py
        --requirements-file
        ${CMAKE_CURRENT_LIST_DIR}/../py/requirements.json
        --download-dir
        ${DOWNLOADS_DIR}
    RESULT_VARIABLE return_code)

# Release the lock:
message(STATUS "Releasing locked resource ${DOWNLOADS_DIR}")
file(LOCK ${DOWNLOADS_DIR} DIRECTORY RELEASE)

if (NOT return_code STREQUAL "0")
    message(FATAL_ERROR "Failed to download resources. Error code ${return_code}")
endif ()
