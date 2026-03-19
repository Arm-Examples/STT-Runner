<!--
SPDX-FileCopyrightText: Copyright 2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
SPDX-License-Identifier: Apache-2.0
-->

# Integration Guide

## Table of Contents

- [Integration Guide](#integration-guide)
- [1) Choose an API](#1-choose-an-api)
- [2) Build the library](#2-build-the-library)
    - [Native build](#native-build)
    - [Android build](#android-build)
- [2.1) Configuration options](#21-configuration-options)
- [3) Package model assets](#3-package-model-assets)
- [4) Embed in your application](#4-embed-in-your-application)
- [5) Performance and memory considerations](#5-performance-and-memory-considerations)
- [6) Android JNI notes](#6-android-jni-notes)
- [7) Compliance checklist](#7-compliance-checklist)

This guide explains how to integrate the **STT-Runner library** into your own applications or products.

The STT-Runner library provides a thin C++ abstraction layer over the selected backend (currently `whisper.cpp`) and enables Arm® KleidiAI™ acceleration by default. The library can be embedded into native applications or accessed from Android via JNI.

---

## 1) Choose an API
Select the API that best fits your application.

| API | When to use it | Output |
| --- | --- | --- |
| C++ API | Native applications and services | Static or shared library |
| JNI API | Android applications | `libarm-stt-jni.so` |
| CLI sample | Quick evaluation and benchmarking | `whisper-cli` executable |

---

## 2) Build the library

Start with a CMake preset and configure the outputs required by your application.

### Native build

```shell
cmake -B build --preset=native -DBUILD_SHARED_LIBS=ON
cmake --build build
```

### Android build

```shell
export NDK_PATH=/path/to/android-ndk
cmake -B build --preset=x-android-aarch64 -DBUILD_SHARED_LIBS=ON
cmake --build build
```

The resulting binaries are located in the `build` directory. To run the generated tests or executables on Android, see:
[Android Build and Execution](../README.md#android-build-and-execution)

---

## 3) Package model assets

The runtime requires a Whisper-compatible model file.

The default model can be downloaded using:

```shell
python scripts/py/download_resources.py
```

For production deployments:

- Place the model file in a **read-only location**.
- Pass the model path to the runtime when initializing the STT-Runner system.

---

## 4) Embed in your application

Minimal integration checklist:

- Link the library into your build system (e.g., `target_link_libraries` in CMake).
- Ensure the model file is accessible at runtime.
- Provide audio input as a **WAV file or compatible audio buffer**.
- Initialize the backend before invoking transcription.
- Log and handle errors during backend initialization and inference.

Example embedding (CMake + C++):

```cmake
# CMakeLists.txt
add_executable(my_app main.cpp)
add_subdirectory(/path/to/STT-Runner stt_runner)
target_link_libraries(my_app PRIVATE arm-stt-cpp)
```

```cpp
// main.cpp
#include "STT.hpp"
#include "WhisperImpl.hpp"
#include <vector>
#include <iostream>

int main() {
    STT<WhisperImpl> stt;
    const char* model_path = "/absolute/path/to/model.bin";
    const char* shared_lib_dir = "/absolute/path/to/build/lib";
    auto* context = stt.InitContext<whisper_context>(model_path, shared_lib_dir);

    stt.InitParams(
        /*printRealTime*/ false,
        /*printProgress*/ false,
        /*timeStamps*/ true,
        /*printSpecial*/ false,
        /*translate*/ false,
        /*language*/ "en",
        /*numThreads*/ 4,
        /*offsetMs*/ 0,
        /*noContext*/ true,
        /*singleSegment*/ false);

    std::vector<float> audio_data = LoadAudioAsFloatPcm("/path/to/audio.wav");
    std::string text = stt.FullTranscribe<whisper_context>(
        context, audio_data.data(), audio_data.size());

    std::cout << text << std::endl;
    stt.FreeContext(context);
    return 0;
}
```

`LoadAudioAsFloatPcm` is a placeholder for your audio loading routine. Use your own WAV loader to produce a mono float PCM buffer compatible with the backend.

---

## 5) Performance and memory considerations

For best performance:

- Use `RelWithDebInfo` builds when profiling.
- Use `Release` builds for production deployments.
- Enable SME acceleration where supported for example:

```
GGML_KLEIDIAI_SME=1 ./build/bin/whisper-cli -m resources_downloaded/models/model.bin /path/to/audio.wav
```

Additional tips:

- Store models on **fast local storage**.
- Avoid loading models repeatedly during application runtime.

---

## 6) Android JNI notes

When integrating with Android:

- Ensure the shared library is packaged with your application.
- Verify the ABI matches the device architecture (`arm64-v8a`).
- Ensure `LD_LIBRARY_PATH` includes the deployed `.so` files when running tests.

Typical location for JNI libraries in Android projects:

```
app/src/main/jniLibs/arm64-v8a/
```

For more information on integrating with Android check out the [Real Time Voce Assistant](https://github.com/Arm-Examples/Real-Time-Voice-Assistant) repo. 

---

## 7) Compliance checklist

When distributing products that include this library:

- Preserve SPDX headers in modified files.
- Include Apache-2.0 licensing notices where required.
- If modifying upstream backends, follow their license requirements.
