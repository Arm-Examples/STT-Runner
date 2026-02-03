#
# SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its affiliates <open-source-office@arm.com>
#
# SPDX-License-Identifier: Apache-2.0
#

include_guard(GLOBAL)

include("${CMAKE_CURRENT_LIST_DIR}/native-base.cmake")

if (USE_KLEIDIAI)
    message(WARNING
        "'KleidiAI=ON' is not supported on x86_64, 'KleidiAI=ON' is an aarch64 only feature. The resulting build may not be stable.")
endif()
