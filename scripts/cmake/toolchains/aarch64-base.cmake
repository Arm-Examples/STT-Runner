#
# SPDX-FileCopyrightText: Copyright 2025-2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
#
# SPDX-License-Identifier: Apache-2.0
#

include_guard(GLOBAL)

if("${TARGET_PLATFORM}"  STREQUAL "linux-aarch64")

  # Following block of code determines CMAKE_C_FLAGS / CMAKE_CXX_FLAGS to be used
  set(_allowed_arches
    Armv8.2_1
    Armv8.2_2
    Armv8.2_3
    Armv8.2_4
    Armv8.2_5
    Armv8.6_1
    Armv8.6_2
    Armv9.2_1
    Armv9.2_2
  )


  list(FIND _allowed_arches "${CPU_ARCH}" _idx)
  if(_idx EQUAL -1)
    list(JOIN _allowed_arches ", " _allowed_str)
    message(WARNING
      "Invalid CPU_ARCH='${CPU_ARCH}'. Supported values: ${_allowed_str}.")
  endif()

  if(CPU_ARCH STREQUAL "Armv8.2_1")
    set(_march "armv8.2-a+dotprod")
  elseif(CPU_ARCH STREQUAL "Armv8.2_2")
    set(_march "armv8.2-a+dotprod+fp16")
  elseif(CPU_ARCH STREQUAL "Armv8.2_3")
    set(_march "armv8.2-a+dotprod+fp16+sve")
  elseif(CPU_ARCH STREQUAL "Armv8.2_4")
    set(_march "armv8.2-a+dotprod+i8mm")
  elseif(CPU_ARCH STREQUAL "Armv8.2_5")
    set(_march "armv8.2-a+dotprod+i8mm+sve+sme")
  elseif(CPU_ARCH STREQUAL "Armv8.6_1")
    set(_march "armv8.6-a+dotprod+fp16+sve+i8mm")
  elseif(CPU_ARCH STREQUAL "Armv8.6_2")
    set(_march "armv8.6-a+dotprod+fp16+sve+i8mm+sve2")
  # ToDo Investigate if this lowercase is intentional or a potential bug.
  elseif(CPU_ARCH STREQUAL "armv9.2_1")
    set(_march "armv9.2-a+dotprod+fp16+sve+i8mm+sme")
  elseif(CPU_ARCH STREQUAL "armv9.2_2")
    set(_march "armv9.2-a+dotprod+fp16+sve+i8mm+sve2+sme")
  else()
  list(JOIN _allowed_arches ", " _allowed_str)
    message(WARNING
      "Could not locate matching -march flags for CPU_ARCH: ${CPU_ARCH}. Suggested values: ${_allowed_str}.")
  endif()

  if(DEFINED _march AND NOT "${_march}" STREQUAL "")
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -march=${_march}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=${_march}")
  endif()

  message(STATUS "CPU_ARCH=${CPU_ARCH} -> -march=${_march}")

endif()
