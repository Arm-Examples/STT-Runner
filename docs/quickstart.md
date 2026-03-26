<!--
SPDX-FileCopyrightText: Copyright 2026 Arm Limited and/or its affiliates <open-source-office@arm.com>

SPDX-License-Identifier: Apache-2.0
-->

# Quick Start

This guide helps you go from a **clean checkout to a working speech transcription** using the STT-Runner library.

The steps below assume a Linux or macOS development environment.

## Table of Contents

- [Quick Start](#quick-start)
- [What you will build](#what-you-will-build)
- [Prerequisites](#prerequisites)
- [Supported Platforms & CMake presets](#supported-platforms--cmake-presets)
  - [Steps](#steps)
    - [1) Clone the repository](#1-clone-the-repository)
    - [2) Download the model and test assets](#2-download-the-model-and-test-assets)
    - [3) Configure a native build](#3-configure-a-native-build)
    - [4) Build the project](#4-build-the-project)
    - [5) Run a transcription](#5-run-a-transcription)
  - [Next Steps](#next-steps)

---

# What you will build

| Output | Description |
| --- | --- |
| `whisper-cli` | Command-line tool that runs speech-to-text transcription on a WAV file. |
| `stt-cpp-tests` | Unit test binary for validating the library (optional). |

---

# Prerequisites

Minimum requirements:

- **CMake 3.27+**
- **Python 3.9+**
- **C++ compiler toolchain**
  - GCC / Clang (Linux)
  - Xcode toolchain (macOS)

Optional platform tools:

- **Android NDK 29.x** for Android builds
- [**aarch64 toolchain**](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) for Linux cross-compilation

---

# Supported Platforms & CMake presets

The supported build platforms and CMake presets matrix is given below. The CMake presets (aka build targets) are given in the first column and the build platform in the first row. For example, `native` builds are tested on Linux-x86_64, Linux-aarch64, and macOS-aarch64. The `x-android-aarch64` preset (targets Android devices running on aarch64) is tested on Linux-x86_64 and macOS-aarch64.

| CMake preset / Host Platform | Linux-x86_64 | Linux-aarch64 | macOS-aarch64 | Android |
| --- | --- | --- | --- | --- |
| `native` | ✅ | ✅  | ✅ | - |
| `x-android-aarch64` | ✅ | - | ✅ | - |
| `x-linux-aarch64` | ✅ | ✅ † | - | - |

† Use `native` preset.


## Steps

### 1) Clone the repository

```shell
git clone https://github.com/Arm-Examples/STT-Runner.git
cd STT-Runner/
```

---

### 2) Download the model and test assets

This repository uses a small default Whisper model for validation.

Downloads are performed automatically during CMake configuration, but you can prefetch them once per checkout:

```shell
python scripts/py/download_resources.py
```

Downloaded assets will appear in:

```
resources_downloaded/
```

Model notes:

- The default model is **ggml-base.en** and is not quantized.
- To reduce compute cost, you can use quantized models such as **Q4_0** via the whisper.cpp quantization tool.
- The default model configuration is declared in `scripts/py/requirements.json`.

---

### 3) Configure a native build

Configure the project using the native CMake preset and enable the example executable:

```shell
cmake -B build --preset=native -DBUILD_EXECUTABLE=ON
```

To also build the test suite:

```shell
cmake -B build --preset=native -DBUILD_EXECUTABLE=ON -DBUILD_UNIT_TESTS=ON
```

---

### 4) Build the project

```shell
cmake -j --build build
```

This produces the binaries under:

```
build/bin/
```

---

### 5) Run a transcription

Run the example CLI with a model and audio file:

```shell
./build/bin/whisper-cli \
    -m resources_downloaded/models/model.bin \
    /path/to/audio/audiofile.wav
```

The tool will output the transcription directly to the console.

If you do not have a WAV file available, you can generate one using any tool that exports **16-bit PCM WAV**.

---


## Next Steps

- See the main repository guide: [README.md](../README.md)
- Performance benchmarking and profiling: [docs/benchmarking.md](benchmarking.md)
- Integration and porting guidance: [docs/integration.md](integration.md)
