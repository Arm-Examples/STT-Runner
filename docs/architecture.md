<!--
    SPDX-FileCopyrightText: Copyright 2026 Arm Limited and/or its affiliates <open-source-office@arm.com>

    SPDX-License-Identifier: Apache-2.0
-->

# Architecture Overview

<!-- TOC -->
  * [System Overview](#system-overview)
      * [Inputs and Outputs](#inputs-and-outputs)
  * [Components](#components)
  * [Execution Flow](#execution-flow)
  * [Supported Build Targets](#supported-build-targets)
  * [Backend Selection](#backend-selection)
<!-- TOC -->

This repository provides a **C++ STT-Runner library** that wraps a selected backend (currently [`whisper.cpp`](https://github.com/ggml-org/whisper.cpp)) and exposes a lightweight C++ API. Optional JNI bindings allow Android™ applications to call the library.

The project uses **CMake presets** to support native builds, Android builds, and cross-compilation targets.

---

## System Overview

```mermaid
graph TD
    AudioInput["Audio Input (WAV)"]

    Java["Java / Android (JNI) - Optional"]
    STTRunner["STT-Runner Wrapper Library (C++)"]
    Backend["Backend Engine (whisper.cpp)"]
    Arm® KleidiAI™["Arm® KleidiAI™ Acceleration (default on Arm)"]
    CPU["CPU / SME Execution"]

    AudioInput --> Java
    AudioInput --> STTRunner

    Java --> STTRunner
    STTRunner --> Backend
    Backend --> Arm® KleidiAI™
    Arm® KleidiAI™ --> CPU

```


### Inputs and Outputs

| Type | Description |
|---|---|
| Input | Audio files (typically **16-bit PCM WAV**). |
| Output | UTF-8 transcription text returned via the CLI or API. |

---

## Components

| Component | Purpose |
|---|---|
| `src/` | Core C++ wrapper implementing the STT-Runner abstraction layer and backend integration. |
| `scripts/py/` | Python utilities for downloading models, test resources, and performing data preparation tasks. |
| `model_configuration_files/` | Model configuration files used by the build system and runtime. |
| `resources_downloaded/` | Default directory where models and example assets are downloaded. |
| `test/` | Unit tests and supporting test resources. |
| `scripts/cmake/` | Toolchains and CMake helper scripts for cross-compilation and platform configuration. |

---

## Execution Flow

1. Audio input (typically a **16-bit PCM WAV file**) is provided to the STT-Runner library.
2. The STT-Runner wrapper forwards the request to whisper.cpp.
3. The backend (`whisper.cpp`) performs inference using the loaded model.
4. If enabled, **Arm® KleidiAI™ kernels accelerate key operations** on supported Arm CPUs.
5. The backend returns the **transcribed text**.
6. Applications receive the result either through:
   - the **C++ API**, or
   - the **JNI interface** on Android.

---

## Supported Build Targets

The project supports multiple build configurations through CMake presets.

| Preset | Platform |
|---|---|
| `native` | Linux / macOS |
| `x-android-aarch64` | Android™ devices using the NDK |
| `x-linux-aarch64` | Cross-compilation for Linux aarch64 targets |

See the **[Quick Start Guide](quickstart.md)** or the main **[README](../README.md)** for build instructions.

---

## Backend Selection

The backend is configured at **CMake configuration time**.

The current implementation expects:

- `whisper.cpp`

Backend configuration options are described in the main **[README](../README.md)**.
