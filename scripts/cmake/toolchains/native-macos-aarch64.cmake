#
# SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its affiliates <open-source-office@arm.com>
#
# SPDX-License-Identifier: Apache-2.0
#

include_guard(GLOBAL)

include("${CMAKE_CURRENT_LIST_DIR}/native-base.cmake")

option(GGML_METAL "Enables MacOS Metal support (GPU hardware acceleration)" OFF)
option(GGML_BLAS "Enables MacOS BLAS (Basic Linear Algebra Subprograms) support (hardware acceleration)" OFF)
