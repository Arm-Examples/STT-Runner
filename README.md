<!--
    SPDX-FileCopyrightText: Copyright 2024-2026 Arm Limited and/or its affiliates <open-source-office@arm.com>

    SPDX-License-Identifier: Apache-2.0
-->

# Speech to Text library
<!-- TOC -->
* [Speech to Text library](#speech-to-text-library)
  * [Prerequisites](#prerequisites)
  * [Configuration options](#configuration-options)
  * [Quick start](#quick-start)
    * [Neural network](#neural-network)
    * [To Build for Android with KleidiAI kernels](#to-build-for-android-with-kleidiai-kernels)
    * [To Build for Android without KleidiAI Kernels](#to-build-for-android-without-kleidiai-kernels)
      * [To test the above build please do the following:](#to-test-the-above-build-please-do-the-following)
    * [To Build for Linux (aarch64) with KleidiAI and SME kernels](#to-build-for-linux-aarch64_with-kleidiai-and-sme-kernels)
    * [To Build for Linux (aarch64) without KleidiAI Kernels](#to-build-for-linux-aarch64_without-kleidiai-kernels)
    * [To Build for native host](#to-build-for-native-host)
    * [To Build for macOS](#to-build-for-macos)
  * [Building and running tests](#building-and-running-tests)
    * [To Build a test executable](#to-build-a-test-executable)
    * [To Build a JNI lib](#to-build-a-jni-lib)
  * [Known Issues](#known-issues)
  * [Trademarks](#trademarks)
  * [License](#license)
<!-- TOC -->

This repo is designed for building an
[Arm® KleidiAI™](https://www.arm.com/markets/artificial-intelligence/software/kleidi)
enabled STT library using CMake build system. It intends to provide an abstraction for [whisper.cpp](https://github.com/ggml-org/whisper.cpp) framework, and it has Arm® KleidiAI™ backend available. In future, we **may** add support for other frameworks and models.

The backend library (selected at CMake configuration stage) is wrapped by this project's thin C++ layer that could be used
directly for testing and evaluations. However, JNI bindings are also provided for developers targeting Android™ based
applications.

## Prerequisites

* CMake 3.27 or above installed
* Android™ NDK 29.0.14033849  (if building for Android™)
* Python 3.9 or above installed, python is used to download test resources and models
* NDK_PATH set to point at the install location of the Android™ NDK
* aarch64 toolchain (Tested with v11.2-2022.02)

## Configuration options

The project is designed to download the required software sources based on user
provided configuration options.

- `STT_DEP_NAME`: Can be `whisper.cpp` only in current implementation. Other options may be added later.
- `WHISPER_SRC_DIR`: Path to the local source directory for the `whisper.cpp` dependency.
- `WHISPER_GIT_URL`: Git repository URL used to clone the `whisper.cpp` dependency.
- `WHISPER_GIT_TAG`: Specific git tag to use for the `whisper.cpp` dependency.

## Quick start

### Preset Build Defaults

CMake presets are used to simplify build commands and the following flags are set by default and must be overwritten as required:
When using the standard presets (native, x-android-aarch64, x-linux-aarch64), the following flags are already set and do NOT need to be passed manually:

-DUSE_KLEIDIAI=ON (unless explicitly overridden)
-DDBUILD_UNIT_TESTS=ON
-DDBUILD_JNI_LIB=ON
-DDBUILD_SHARED_LIBS=OFF
-DDBUILD_EXECUTABLE=OFF
-DDGGML_OPENMP=OFF

macOS Specific:
-DGGML_METAL=OFF
-DGGML_BLAS=OFF

## Supported Platforms & cmake presets

The supported build platforms and cmake presets matrix is given below.
The cmake presets (aka build target) are given in the first column and build platform in the first row.
So for example native builds are have been tested on Linux-x86_64, Linux-aarch64 & macOS-aarch64. While x-android-aarch64 (targets Android™ devices running on aarch64) builds are only tested on Linux-x86_64 & macOS-aarch64.

|  cmake-preset / Host Platform  | Linux-x86_64| Linux-aarch64                      | macOS-aarch64 | Android™ |
|--------------------------------------|---------------|------------------------------------|---------------|---------|
| native                               | ✅            | ✅ *                              | ✅            | -      |
| x-android-aarch64                    | ✅            | -                                 | ✅            | -      |
| x-linux-aarch64                      | ✅            | ✅ †                              | -            | -      |


† Use 'native' preset

### To Build for Linux (aarch64) with KleidiAI and SME kernels

Instead of passing a complex -march=… string directly to CMake, CPU_ARCH can be set to one of the canonical names e.g. -DCPU_ARCH=Armv8.2_4
The build system will map CPU_ARCH to the correct -march= feature string for you and validate the value. 
If you do want to supply a custom -march exactly as-is, pass it through the GGML_CPU_ARM_ARCH variable:
 -DGGML_CPU_ARM_ARCH="armv8.2-a+dotprod+i8mm")

Supported CPU_ARCH values: Armv8.2_1, Armv8.2_2, Armv8.2_3, Armv8.2_4, Armv8.2_5, Armv8.6_1, Armv8.6_2, Armv9.2_1, Armv9.2_2.

To build with SME kernels, ensure `GGML_CPU_ARM_ARCH` is set with needed feature flags as below.
Flag USE_KLEIDI is set to ON by default and will set -DGGML_CPU_KLEIDIAI=ON automatically.

```shell

 export TOOLCHAIN=/home/user/tools/arm-gnu-toolchain-14.3.rel1-aarch64-aarch64-none-linux-gnu
 export PATH="$TOOLCHAIN/bin:$PATH"

cmake -B build \
    --preset=x-linux-aarch64 \
    -DCPU_ARCH=Armv8.2_2 \

cmake --build ./build
```

Once built, a standalone application can be executed to get performance.

Set `GGML_KLEIDIAI_SME=1` to enable the use of SME kernels during execution:

```shell
GGML_KLEIDIAI_SME=1 ./build/bin/whisper-cli -m resources_downloaded/models/model.bin /path/to/audio/audiofile.wav
```

To run without invoking SME kernels, set `GGML_KLEIDIAI_SME=0` during execution:

```shell
GGML_KLEIDIAI_SME=0 ./build/bin/whisper-cli -m resources_downloaded/models/model.bin /path/to/audio/audiofile.wav
```

### To Build for native host

Default build is a release build with tests but these can be manually toggled using appropriate flags

## Building with CMake presets

The following examples show how to configure the project using the available
top-level presets. Pick one configure command, then run the common build
command below. Default settings are for release builds with unit-tests and jni enabled. Shared libraries are disabled.

```shell
# Native host (GNU toolchain)
cmake -B build --preset=native

# Android (arm64, NDK)
cmake -B build --preset=x-android-aarch64

# Linux (aarch64 cross-compile)
cmake -B build --preset=x-linux-aarch64

# Common build command
cmake --build build
```

### Overriding preset options at configure time (non-exhaustive list)

You can override preset defaults by passing `-D<VAR>=<VALUE>` alongside `--preset`.


```sh
# Disable unit tests
cmake -B build --preset=native -DBUILD_UNIT_TESTS=OFF

# Switch to a Debug build
cmake -B build --preset=native -DCMAKE_BUILD_TYPE=Debug

# Disable unit tests and use a Debug build
cmake -B build --preset=native -DBUILD_UNIT_TESTS=OFF -DCMAKE_BUILD_TYPE=Debug
```

### To Build for Linux (aarch64) without KleidiAI Kernels

```shell
cmake -B build \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/aarch64.cmake \
    -DCMAKE_C_FLAGS=-march=armv8.2-a+dotprod+i8mm+fp16 \
    -DCMAKE_CXX_FLAGS=-march=armv8.2-a+dotprod+i8mm+fp16 \
    -DGGML_CPU_KLEIDIAI=OFF
cmake --build ./build
```

### To Build for macOS
To build for the CPU backend on macOS®, you can use the native CMake preset which will automatically assign toolchain.

```shell
cmake -B build --preset=native \
    -DCMAKE_C_FLAGS=-march=armv8.2-a+dotprod+i8mm+fp16 \
    -DCMAKE_CXX_FLAGS=-march=armv8.2-a+dotprod+i8mm+fp16 \
cmake --build ./build
```

## Building and running tests

To build and test for native host machine:

```shell
cmake -B build --preset=native
cmake --build ./build
ctest --test-dir ./build
```

### To Build a test executable

The option:
```
-DBUILD_EXECUTABLE=true
```

For example

  ```shell
cmake -B build \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/aarch64.cmake \
    -DCMAKE_C_FLAGS=-march=armv8.2-a+dotprod+i8mm+fp16 \
    -DCMAKE_CXX_FLAGS=-march=armv8.2-a+dotprod+i8mm+fp16 \
    -DBUILD_EXECUTABLE=true
cmake --build ./build
```

This will produce an executable, which you can use to test your build under :
```
/build/bin/whisper-cli
```

You can run this executable and test an audio file using the following:
```
./whisper-cli -m resources_downloaded/models/model.bin /path/to/audio/audiofile.wav
```

### To Build a JNI lib

Add the options:
```
-DBUILD_JNI_LIB=true
```

to run the sample test `WhisperTestApp.java` run the following commands post-build
```
ctest --test-dir ./build
```

### To Build for Android with KleidiAI kernels

```shell

export NDK_PATH=/path/to/android-ndk-r29

cmake -B build \
    -DCMAKE_TOOLCHAIN_FILE=${NDK_PATH}/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-33 \
    -DGGML_CPU_ALL_VARIANTS=ON \
    -DGGML_BACKEND_DL=ON \
    -DGGML_SYSTEM_ARCH=ARM \
    -DBUILD_SHARED_LIBS=ON \
    -DTEST_DATA_DIR="/data/local/tmp" \
    -DTEST_MODELS_DIR="/data/local/tmp" \
    -DBACKEND_SHARED_LIB_DIR="/data/local/tmp" \
    -DGGML_OPENMP=OFF

cmake --build ./build
```

### To Build for Android without KleidiAI Kernels

```shell
cmake -B build \
    -DCMAKE_TOOLCHAIN_FILE=${NDK_PATH}/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-33 \
    -DGGML_CPU_ALL_VARIANTS=ON \
    -DGGML_BACKEND_DL=ON \
    -DGGML_SYSTEM_ARCH=ARM \
    -DBUILD_SHARED_LIBS=ON \
    -DGGML_CPU_KLEIDIAI=OFF \
    -DTEST_DATA_DIR="/data/local/tmp" \
    -DTEST_MODELS_DIR="/data/local/tmp" \
    -DBACKEND_SHARED_LIB_DIR="/data/local/tmp" \
    -DGGML_OPENMP=OFF

cmake --build ./build
```

#### To test the above build please do the following:

Firstly push the newly built test executable:
```shell
adb push build/bin/stt-cpp-tests /data/local/tmp
```

Next push the newly built libs:
```shell
adb push build/lib/* /data/local/tmp
```

Next you will need to shell to your device:
```shell
adb shell
cd data/local/tmp
export LD_LIBRARY_PATH=./
```

Finally just run the test executable:
```shell
./stt-cpp-tests
```

### Underlying Neural network used in transcription

This project uses the **ggml-base.en model** as its default network. The model is not quantized.
To strike a balance between computational efficiency and model performance, you can use the **Q4_0 quantization format**.
To quantize your model, you can use the [whisper.cpp quantize tool](https://github.com/ggml-org/whisper.cpp/tree/v1.7.4?tab=readme-ov-file#quantization).

- You can access the model from [Hugging Face](https://huggingface.co/ggerganov/whisper.cpp/blob/main/ggml-base.en.bin).
- The default model configuration is declared in the [`requirements.json`](scripts/py/requirements.json) file.

However, any model supported by the backend library could be used.
> **NOTE**: Currently only Q4_0 models are accelerated by Arm® KleidiAI™ kernels in whisper.cpp.

## Known Issues
**Transcription speed** - We are working on improving this, approximately 2x slower than expected currently

## Trademarks

* Arm® and KleidiAI™ are registered trademarks or trademarks of Arm® Limited (or its subsidiaries) in the US and/or
  elsewhere.
* Android™ is a trademark of Google LLC.
* macOS® is a trademark of Apple Inc.

## License

This project is distributed under the software licenses in [LICENSES](LICENSES) directory.
