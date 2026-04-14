<!--
    SPDX-FileCopyrightText: Copyright 2026 Arm Limited and/or its affiliates <open-source-office@arm.com>

    SPDX-License-Identifier: Apache-2.0
-->

# Benchmarking

This guide shows how to capture repeatable performance numbers with the standalone STT benchmark executable and where to use Arm Streamline and Perfetto for profiling, including SME-enabled runs.

## 1) Define a baseline workload

Pick a representative benchmark configuration and keep it constant across runs.

Record:

- Model: `path/to/model`
- Synthetic audio duration or input sample count
- Build configuration (preset, flags)
- Runtime flags

Example baseline:

- Model: `resources_downloaded/models/model.bin`
- Synthetic audio duration: `--duration 11`
- Build configuration: `native`, `Release`, `BUILD_BENCHMARK=ON`
- Runtime flags: `--threads 4`, `GGML_KLEIDIAI_SME=0`


## 2) Build for profiling

Use a build that preserves symbols while remaining optimized:

```shell
cmake -B build --preset=native -DCMAKE_BUILD_TYPE=Release -DBUILD_BENCHMARK=ON
cmake --build build --target stt-bench
```

For SME-capable aarch64 targets, set a supported CPU architecture and use the KleidiAI backend (enabled by default in presets):

```shell
cmake -B build --preset=x-linux-aarch64 -DGGML_CPU_ARM_ARCH=armv8.2-a+dotprod+i8mm+sve+sme -DBUILD_BENCHMARK=ON
cmake --build build --target stt-bench
```

## 3) Run baseline measurements

Measure wall-clock time and capture the benchmark report:

```shell
/usr/bin/time -v ./build/bin/stt-bench --model resources_downloaded/models/model.bin --duration 11 --threads 4 --iterations 5 --warmup 1
```

Collect at least 3 runs and report the median.

## 4) Compare SME on/off

SME kernels are enabled at runtime:

```shell
GGML_KLEIDIAI_SME=1 ./build/bin/stt-bench --model resources_downloaded/models/model.bin --duration 11 --threads 4 --iterations 5 --warmup 1
```

Disable to compare:

```shell
GGML_KLEIDIAI_SME=0 ./build/bin/stt-bench --model resources_downloaded/models/model.bin --duration 11 --threads 4 --iterations 5 --warmup 1
```

Record the delta in latency and throughput.

## 5) Profile with Arm Streamline

Streamline (part of Arm Performance Studio) provides CPU, memory, and system-level insights. A typical capture flow:

To enable additional timeline annotations, configure the build with:

```shell
cmake -B build --preset=native -DENABLE_STREAMLINE=ON
cmake --build build
```

When enabled, CMake fetches Arm Gator annotation sources and adds markers around the STT-Runner wrapper lifecycle and JNI control-path entry points.

1. Install Arm Performance Studio on your host and ensure the target has the Streamline data capture service (gator/streamline agent) installed.
2. Build the library.
3. Launch Streamline, create a new capture, and select the target device.
4. Select metrics for CPU, cache, and memory bandwidth. Where available, enable SVE/SME-related counters.
5. Start capture, run the `stt-bench` workload, then stop capture.
6. Inspect hotspots, core utilization, and memory pressure.

Use the capture to identify whether the workload is compute-bound, memory-bound, or impacted by scheduling.

## 6) Trace with Perfetto (SME analysis)

Perfetto helps analyze scheduling, CPU frequency changes, and system events.

1. Install Perfetto on the target or ensure it is available in PATH.
2. Create a trace config that includes CPU scheduling and frequency events.
3. Start the trace, run `stt-bench`, then stop the trace.
4. Open the trace in the Perfetto UI to inspect CPU timelines and task slices.

For more information please see:
[Perfetto CPU Profiling Guide](https://perfetto.dev/docs/getting-started/cpu-profiling)

## 7) Reporting template

Capture the following for each run:

| Field | Example |
| --- | --- |
| Platform | Linux aarch64 (cross-compiled) |
| Build preset | `x-linux-aarch64` |
| Build type | `RelWithDebInfo` |
| Model | `ggml-base.en` |
| Synthetic audio duration | 30s |
| SME | On/Off |
| Median latency | 2.1s |
| Peak RSS | 1.2 GB |
| Notes | SMT on, CPU governor performance |
