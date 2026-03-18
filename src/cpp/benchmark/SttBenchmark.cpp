//
// SPDX-FileCopyrightText: Copyright 2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#include "SttBenchmark.hpp"

#include "Logger.hpp"
#include "STT.hpp"
#include "WhisperImpl.hpp"

#include <chrono>
#include <cmath>
#include <iomanip>
#include <random>
#include <sstream>
#include <stdexcept>
#include <utility>

using namespace std::chrono;

namespace {

// Whisper.cpp backend utilities which can't be obtained through STT at the moment
using BenchmarkImpl = WhisperImpl;
using BenchmarkContext = whisper_context;

constexpr int kBackendSampleRateHz = WHISPER_SAMPLE_RATE;
constexpr const char* kBackendName = "whisper.cpp";

std::vector<float> GenerateBenchmarkAudio(const std::size_t numSamples)
{
    if (numSamples == 0) {
        throw std::invalid_argument("Benchmark input must contain at least one sample.");
    }

    std::mt19937 generator(42);
    std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

    std::vector<float> samples(numSamples, 0.0f);
    for (float& sample : samples) {
        sample = distribution(generator);
    }

    return samples;
}

std::string Pad(const std::string& value, const std::size_t width)
{
    if (value.size() >= width) {
        return value.substr(0, width);
    }
    return value + std::string(width - value.size(), ' ');
}

std::string FormatMetric(const double mean, const double stddev, const char* unit)
{
    std::ostringstream stream;
    stream << std::fixed << std::setw(9) << std::setprecision(3) << mean
           << " ± " << std::setw(7) << std::setprecision(3) << stddev
           << " (" << unit << ")";
    return stream.str();
}

std::string FormatValue(const double value, const char* unit)
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(3) << value << " (" << unit << ")";
    return stream.str();
}

} // namespace

struct SttBenchmark::BackendData {
    STT<BenchmarkImpl> stt;
    BenchmarkContext* context = nullptr;
};

SttBenchmark::SttBenchmark(SttBenchmarkConfig config)
    : m_config(std::move(config))
    , m_backend(std::make_unique<BackendData>())
{}

SttBenchmark::~SttBenchmark() noexcept
{
    ReleaseContext();
}

int SttBenchmark::GetInputSampleRateHz()
{
    return kBackendSampleRateHz;
}

void SttBenchmark::ValidateConfig() const
{
    if (m_config.modelPath.empty()) {
        throw std::invalid_argument("Benchmark requires a model path.");
    }
    if (m_config.numSamples == 0) {
        throw std::invalid_argument("Benchmark input must contain at least one sample.");
    }
    if (m_config.numThreads <= 0) {
        throw std::invalid_argument("Benchmark threads must be positive.");
    }
    if (m_config.numIterations <= 0) {
        throw std::invalid_argument("Benchmark iterations must be positive.");
    }
    if (m_config.numWarmupIterations < 0) {
        throw std::invalid_argument("Benchmark warm-up iterations cannot be negative.");
    }
}

void SttBenchmark::InitStt()
{
    m_backend->stt.InitParams(m_config.printRealtime,
                              m_config.printProgress,
                              m_config.timeStamps,
                              m_config.printSpecial,
                              m_config.translate,
                              m_config.language.c_str(),
                              m_config.numThreads,
                              m_config.offsetMs,
                              m_config.noContext,
                              m_config.singleSegment);

    const auto loadStart = steady_clock::now();
    m_backend->context = m_backend->stt.InitContext<BenchmarkContext>(m_config.modelPath.c_str(),
                                                                      m_config.sharedLibraryPath.c_str());
    const auto loadEnd = steady_clock::now();
    m_loadTimeMs = duration<double, std::milli>(loadEnd - loadStart).count();
}

void SttBenchmark::ReleaseContext() noexcept
{
    if (m_backend && m_backend->context != nullptr) {
        try {
            m_backend->stt.FreeContext<BenchmarkContext>(m_backend->context);
        } catch (...) {
            LOG_WARN("Ignoring exception while freeing benchmark context");
        }
        m_backend->context = nullptr;
    }
}

SttBenchmark::IterationResult SttBenchmark::RunSingleIteration()
{
    if (m_backend->context == nullptr) {
        throw std::runtime_error("Benchmark context is not initialised.");
    }

    const auto start = steady_clock::now();
    const std::string transcription =
        m_backend->stt.FullTranscribe<BenchmarkContext>(m_backend->context,
                                                        m_audioSamples.data(),
                                                        static_cast<int>(m_audioSamples.size()));
    const auto end = steady_clock::now();

    IterationResult result;
    result.totalTimeMs = duration<double, std::milli>(end - start).count();

    if (const auto* timings = whisper_get_timings(m_backend->context); timings != nullptr) {
        result.encodeTimeMs = timings->encode_ms;
        result.decodeTimeMs = timings->decode_ms;
    }

    result.transcriptionChars = static_cast<double>(transcription.size());

    const double audioDurationSec =
        static_cast<double>(m_audioSamples.size()) / kBackendSampleRateHz;
    const double benchmarkDurationSec = result.totalTimeMs / 1000.0;

    if (audioDurationSec > 0.0) {
        result.realtimeFactor = benchmarkDurationSec / audioDurationSec;
    }
    if (benchmarkDurationSec > 0.0) {
        result.samplesPerSecond = static_cast<double>(m_audioSamples.size()) / benchmarkDurationSec;
    }

    return result;
}

SttBenchmark::SummaryStats SttBenchmark::ComputeSummaryStats() const
{
    SummaryStats stats{};
    if (m_results.empty()) {
        return stats;
    }

    auto accumulateMetric = [&](auto getter) {
        double sum = 0.0;
        for (const auto& result : m_results) {
            sum += getter(result);
        }
        return sum / m_results.size();
    };

    stats.mean.totalTimeMs = accumulateMetric([](const auto& value) { return value.totalTimeMs; });
    stats.mean.encodeTimeMs = accumulateMetric([](const auto& value) { return value.encodeTimeMs; });
    stats.mean.decodeTimeMs = accumulateMetric([](const auto& value) { return value.decodeTimeMs; });
    stats.mean.realtimeFactor = accumulateMetric([](const auto& value) { return value.realtimeFactor; });
    stats.mean.samplesPerSecond = accumulateMetric([](const auto& value) { return value.samplesPerSecond; });
    stats.mean.transcriptionChars = accumulateMetric([](const auto& value) { return value.transcriptionChars; });

    auto computeStddev = [&](auto getter, const double mean) {
        double accum = 0.0;
        for (const auto& result : m_results) {
            const double delta = getter(result) - mean;
            accum += delta * delta;
        }
        return std::sqrt(accum / m_results.size());
    };

    stats.stddev.totalTimeMs = computeStddev([](const auto& value) { return value.totalTimeMs; },
                                             stats.mean.totalTimeMs);
    stats.stddev.encodeTimeMs = computeStddev([](const auto& value) { return value.encodeTimeMs; },
                                              stats.mean.encodeTimeMs);
    stats.stddev.decodeTimeMs = computeStddev([](const auto& value) { return value.decodeTimeMs; },
                                              stats.mean.decodeTimeMs);
    stats.stddev.realtimeFactor = computeStddev([](const auto& value) { return value.realtimeFactor; },
                                                stats.mean.realtimeFactor);
    stats.stddev.samplesPerSecond = computeStddev([](const auto& value) { return value.samplesPerSecond; },
                                                  stats.mean.samplesPerSecond);
    stats.stddev.transcriptionChars = computeStddev([](const auto& value) { return value.transcriptionChars; },
                                                    stats.mean.transcriptionChars);

    return stats;
}

int SttBenchmark::Run()
{
    try {
        ValidateConfig();
        ReleaseContext();
        m_results.clear();
        m_loadTimeMs = 0.0;
        m_audioSamples = GenerateBenchmarkAudio(m_config.numSamples);

        InitStt();

        for (int warmupIndex = 0; warmupIndex < m_config.numWarmupIterations; ++warmupIndex) {
            static_cast<void>(RunSingleIteration());
        }

        m_results.reserve(static_cast<std::size_t>(m_config.numIterations));
        for (int iterationIndex = 0; iterationIndex < m_config.numIterations; ++iterationIndex) {
            m_results.push_back(RunSingleIteration());
        }

        return 0;
    } catch (const std::exception& ex) {
        LOG_ERROR("Benchmark failed: %s", ex.what());
        return 1;
    } catch (...) {
        LOG_ERROR("Benchmark failed: unknown error");
        return 1;
    }
}

std::string SttBenchmark::GetResults() const
{
    if (m_results.empty()) {
        return "No benchmark results available. Run() has not been executed yet.\n";
    }

    const auto stats = ComputeSummaryStats();
    const double audioDurationMs =
        1000.0 * static_cast<double>(m_audioSamples.empty() ? m_config.numSamples : m_audioSamples.size()) /
        kBackendSampleRateHz;

    constexpr std::size_t colBackend = 18;
    constexpr std::size_t colThreads = 7;
    constexpr std::size_t colTest = 8;
    constexpr std::size_t colPerf = 28;

    std::ostringstream output;
    output << "\n=== ARM STT Benchmark ===\n\n";
    output << "Parameters:\n";
    output << "  backend            : " << kBackendName << "\n";
    output << "  model_path         : " << m_config.modelPath << "\n";
    output << "  sample_rate_hz     : " << kBackendSampleRateHz << "\n";
    output << "  num_samples        : " << (m_audioSamples.empty() ? m_config.numSamples : m_audioSamples.size()) << "\n";
    output << "  audio_duration_ms  : " << std::fixed << std::setprecision(3) << audioDurationMs << "\n";
    output << "  num_threads        : " << m_config.numThreads << "\n";
    output << "  num_iterations     : " << m_config.numIterations << "\n";
    output << "  num_warmup         : " << m_config.numWarmupIterations << "\n\n";

    output << "======= Results =======\n\n";
    output << "| " << Pad("Backend", colBackend)
           << " | " << Pad("Threads", colThreads)
           << " | " << Pad("Test", colTest)
           << " | " << Pad("Performance", colPerf) << " |\n";
    output << "| " << std::string(colBackend, '-')
           << " | " << std::string(colThreads, '-')
           << " | " << std::string(colTest, '-')
           << " | " << std::string(colPerf, '-') << " |\n";

    const std::string threads = std::to_string(m_config.numThreads);
    output << "| " << Pad(kBackendName, colBackend)
           << " | " << Pad(threads, colThreads)
           << " | " << Pad("Load", colTest)
           << " | " << Pad(FormatValue(m_loadTimeMs, "ms"), colPerf) << " |\n";
    output << "| " << Pad(kBackendName, colBackend)
           << " | " << Pad(threads, colThreads)
           << " | " << Pad("Total", colTest)
           << " | " << Pad(FormatMetric(stats.mean.totalTimeMs, stats.stddev.totalTimeMs, "ms"), colPerf)
           << " |\n";
    output << "| " << Pad(kBackendName, colBackend)
           << " | " << Pad(threads, colThreads)
           << " | " << Pad("Encode", colTest)
           << " | " << Pad(FormatMetric(stats.mean.encodeTimeMs, stats.stddev.encodeTimeMs, "ms"), colPerf)
           << " |\n";
    output << "| " << Pad(kBackendName, colBackend)
           << " | " << Pad(threads, colThreads)
           << " | " << Pad("Decode", colTest)
           << " | " << Pad(FormatMetric(stats.mean.decodeTimeMs, stats.stddev.decodeTimeMs, "ms"), colPerf)
           << " |\n";
    output << "| " << Pad(kBackendName, colBackend)
           << " | " << Pad(threads, colThreads)
           << " | " << Pad("RTF", colTest)
           << " | " << Pad(FormatMetric(stats.mean.realtimeFactor, stats.stddev.realtimeFactor, "x"), colPerf)
           << " |\n";
    output << "| " << Pad(kBackendName, colBackend)
           << " | " << Pad(threads, colThreads)
           << " | " << Pad("Samp/s", colTest)
           << " | " << Pad(FormatMetric(stats.mean.samplesPerSecond,
                                        stats.stddev.samplesPerSecond,
                                        "s/s"),
                           colPerf)
           << " |\n";
    output << "| " << Pad(kBackendName, colBackend)
           << " | " << Pad(threads, colThreads)
           << " | " << Pad("Chars", colTest)
           << " | " << Pad(FormatMetric(stats.mean.transcriptionChars,
                                        stats.stddev.transcriptionChars,
                                        "chars"),
                           colPerf)
           << " |\n";

    return output.str();
}
