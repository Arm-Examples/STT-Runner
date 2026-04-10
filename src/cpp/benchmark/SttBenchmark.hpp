//
// SPDX-FileCopyrightText: Copyright 2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

/**
 * Runtime configuration for a single STT benchmark run.
 */
struct SttBenchmarkConfig {
    std::string modelPath;          ///< Model file to load.
    std::string sharedLibraryPath;  ///< Directory used by backend dynamic loading.
    std::size_t numSamples = 0;     ///< Number of synthetic input samples per iteration.
    int numThreads = 0;             ///< Runtime thread count passed into InitParams(); must be set explicitly.
    int numIterations = 5;          ///< Number of measured iterations.
    int numWarmupIterations = 1;    ///< Warm-up iterations excluded from the report.
    bool printRealtime = false;     ///< Forward realtime output flag to the backend.
    bool printProgress = false;     ///< Forward progress output flag to the backend.
    bool timeStamps = false;        ///< Forward timestamp output flag to the backend.
    bool printSpecial = false;      ///< Forward special token output flag to the backend.
    bool translate = false;         ///< Forward translation flag to the backend.
    std::string language = "en";    ///< Language code passed into InitParams().
    int offsetMs = 0;               ///< Initial audio offset in milliseconds.
    bool noContext = false;         ///< Disable cross-segment context reuse.
    bool singleSegment = false;     ///< Force a single-segment decode.
};

/**
 * Minimal benchmark wrapper around the generic STT interface.
 */
class SttBenchmark {
public:
    /**
     * Per-iteration benchmark statistics.
     */
    struct IterationResult {
        double totalTimeMs = 0.0;
        double encodeTimeMs = 0.0;
        double decodeTimeMs = 0.0;
        double realtimeFactor = 0.0;
        double samplesPerSecond = 0.0;
        double transcriptionChars = 0.0;
    };

    explicit SttBenchmark(SttBenchmarkConfig config);
    ~SttBenchmark() noexcept;

    SttBenchmark(const SttBenchmark&) = delete;
    SttBenchmark& operator=(const SttBenchmark&) = delete;

    /**
     * Run the benchmark end to end.
     * @return 0 on success, non-zero on failure.
     */
    int Run();

    /**
     * Return a human-readable benchmark report.
     */
    std::string GetResults() const;

    /**
     * Return the benchmark input sample rate expected by the current backend.
     */
    static int GetInputSampleRateHz();

private:
    struct SummaryStats {
        IterationResult mean{};
        IterationResult stddev{};
    };

    struct BackendData;

    void ValidateConfig() const;
    void InitStt();
    void ReleaseContext() noexcept;
    IterationResult RunSingleIteration();
    SummaryStats ComputeSummaryStats() const;

    SttBenchmarkConfig m_config;
    std::unique_ptr<BackendData> m_backend;
    std::vector<float> m_audioSamples;
    double m_loadTimeMs = 0.0;
    std::vector<IterationResult> m_results;
};
