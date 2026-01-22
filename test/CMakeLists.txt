#
# SPDX-FileCopyrightText: Copyright 2024-2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
#
# SPDX-License-Identifier: Apache-2.0
#

# Declare project
project(stt-tests
        VERSION     0.0.1
        DESCRIPTION "STT wrapper tests")

set(TEST_TARGET_NAME stt-cpp-tests)
set(CATCH_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE STRING "Catch2 location")

file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources)

set(TEST_DATA_DIR           ${CMAKE_CURRENT_SOURCE_DIR}/resources CACHE STRING "Directory with required input data resources")
set(TEST_MODELS_DIR         ${DOWNLOADS_DIR}/models CACHE STRING "Directory with required model resources")
set(TEST_UTILS_DIR          ${DOWNLOADS_DIR}/test_utils CACHE STRING "Directory with required test utilities jar resources")
set(BACKEND_SHARED_LIB_DIR  ${CMAKE_BINARY_DIR}/bin CACHE STRING "Directory with linked libraries")

if (NOT TARGET arm-stt-cpp)
    message(FATAL_ERROR "stt-cpp-tests requires target arm-stt-cpp")
endif()

message(STATUS "Adding C++ tests")
add_executable(stt-cpp-tests
    cpp/WhisperTest.cpp
    cpp/SttLogTest.cpp)

# Convert WAV file to CSV for testing
include(convert-wav)

# We pass in the compile definition for location of test
# models directory and test data directory for input data file
target_compile_definitions(stt-cpp-tests
    PUBLIC
    TEST_MODELS_DIR="${TEST_MODELS_DIR}"
    TEST_DATA_DIR="${TEST_DATA_DIR}"
    BACKEND_SHARED_LIB_DIR="${BACKEND_SHARED_LIB_DIR}")

# Android specific definition for Catch
if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    target_compile_definitions(stt-cpp-tests PRIVATE
        "CATCH_CONFIG_NO_ANDROID_LOGWRITE")
endif()

# Assert dependency on the actual C++ interface or library.
target_link_libraries(stt-cpp-tests PUBLIC arm-stt-cpp)

# Download Catch and add to the build.
include(find-catch)

# Include Catch header directory.
target_include_directories(stt-cpp-tests PUBLIC ${CATCH_DIR})

# Add test to CTest for running it automatically
enable_testing()
add_test(NAME stt-cpp-ctest COMMAND stt-cpp-tests)

# If JNI libs are being built, add tests for these here.
if(TARGET arm-stt-jni)
    message(STATUS "Adding JNI tests")

    if (NOT TARGET arm-stt-java)
        message(FATAL_ERROR "JNI tests cannot be built without Java interface")
    endif()

    # Find JAVA and add UseJava CMake module.
    include(find-java)

    # Find all the JAR files (dependencies) that have been downloaded.
    file(GLOB _jarDependencies "${TEST_UTILS_DIR}/*.jar")

    # Get the Java interface source file path.
    get_target_property(STT_JAVA_INTERFACE_SOURCES
        arm-stt-java INTERFACE_SOURCES)
    message(STATUS "Java interface sources: ${STT_JAVA_INTERFACE_SOURCES}")

    # Build Java archive using the interface and test sources.
    add_jar(
        stt-jni-tests
            ${STT_JAVA_INTERFACE_SOURCES}
            ${CMAKE_CURRENT_SOURCE_DIR}/java/com/arm/stt/WhisperTestApp.java
        INCLUDE_JARS
            ${_jarDependencies})

    get_target_property(_jarFile stt-jni-tests JAR_FILE)
    get_target_property(_classDir stt-jni-tests CLASSDIR)
    get_target_property(_jniLibPath arm-stt-jni LIBRARY_OUTPUT_DIRECTORY)

    message(STATUS "Jar file:          ${_jarFile}")
    message(STATUS "Class compiled to: ${_classDir}")
    message(STATUS "JNI lib directory: ${_jniLibPath}")
    message(STATUS "Jar dependencies:  ${_jarDependencies}")

    string(REPLACE ";" ":" _jarDependenciesTests "${_jarDependencies}")
    add_test(
        NAME whisper-jni-ctest
        COMMAND
            ${Java_JAVA_EXECUTABLE}
            -Djava.library.path=${_jniLibPath}
            -Dbackend.shared.lib.dir=${CMAKE_BINARY_DIR}/bin
            -Dmodel_dir=${TEST_MODELS_DIR}
            -Dtest_data_dir=${TEST_DATA_DIR}
            -cp ${_jarDependenciesTests}:${_jarFile}
            org.junit.runner.JUnitCore
            com.arm.stt.WhisperTestApp)
endif()
