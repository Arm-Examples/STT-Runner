#
# SPDX-FileCopyrightText: Copyright 2025-2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
#
# SPDX-License-Identifier: Apache-2.0
#

# This file is the first step in automatically deducing host and configuring build.
# It dispatches to appropriate native-x86,aarch64 or macOS cmakes which in turn include native-base.

include_guard(GLOBAL)

# Optional override: -D TARGET_PLATFORM=macos|linux-x86_64|linux-aarch64
set(TARGET_PLATFORM "${TARGET_PLATFORM}" CACHE STRING "Force native selection: macos, linux-x86_64, linux-aarch64")
if(NOT TARGET_PLATFORM AND DEFINED ENV{TARGET_PLATFORM})
  set(TARGET_PLATFORM "$ENV{TARGET_PLATFORM}")
endif()

if(NOT TARGET_PLATFORM)
  set(_host "${CMAKE_HOST_SYSTEM_NAME}")
  string(TOLOWER "${CMAKE_HOST_SYSTEM_PROCESSOR}" _proc)

  if(_host STREQUAL "Darwin")
    set(TARGET_PLATFORM "macos")
  elseif(_host STREQUAL "Linux")
    if(_proc MATCHES "^(x86_64|amd64)$")
      set(TARGET_PLATFORM "linux-x86_64")
    elseif(_proc MATCHES "^(aarch64|arm64)$")
      set(TARGET_PLATFORM "linux-aarch64")
    else()
      message(FATAL_ERROR "native-dispatch: unsupported Linux processor '${CMAKE_HOST_SYSTEM_PROCESSOR}'")
    endif()
  else()
    message(FATAL_ERROR "native-dispatch: unsupported host '${CMAKE_HOST_SYSTEM_NAME}'")
  endif()
endif()

set(NATIVE_SELECTED "${TARGET_PLATFORM}" CACHE STRING "Resolved native selection")

if(TARGET_PLATFORM STREQUAL "macos")
  include("${CMAKE_CURRENT_LIST_DIR}/native-macos-aarch64.cmake")
elseif(TARGET_PLATFORM STREQUAL "linux-x86_64")
  include("${CMAKE_CURRENT_LIST_DIR}/native-linux-x86_64.cmake")
elseif(TARGET_PLATFORM STREQUAL "linux-aarch64")
  include("${CMAKE_CURRENT_LIST_DIR}/native-linux-aarch64.cmake")
else()
  message(FATAL_ERROR "native-dispatch: unknown TARGET_PLATFORM='${TARGET_PLATFORM}'")
endif()

message(STATUS "native-dispatch: selected '${NATIVE_SELECTED}'")
