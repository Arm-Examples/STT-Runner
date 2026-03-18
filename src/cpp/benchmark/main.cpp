//
// SPDX-FileCopyrightText: Copyright 2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#include "SttBenchmark.hpp"

#include "Logger.hpp"

#include <cmath>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

struct CliOptions {
    std::string modelPath;
    std::string sharedLibraryPath;
    int numThreads = 0;
    int numIterations = 5;
    int numWarmupIterations = 1;
    int offsetMs = 0;
    std::size_t targetSamples = 0;
    double durationSeconds = 0.0;
    bool printRealtime = false;
    bool printProgress = false;
    bool timeStamps = false;
    bool printSpecial = false;
    bool translate = false;
    bool noContext = false;
    bool singleSegment = false;
    std::string language = "en";
};

void PrintUsage(const char* program)
{
    std::cerr << "\nSTT Benchmark Tool\n";
    std::cerr << "Usage:\n";
    std::cerr << "  " << program
              << " --model <model_path>"
              << " --threads <n>"
              << " --duration <seconds> | --samples <count>"
              << " [--iterations <n>] [--warmup <n>]"
              << " [--language <code>] [--shared-libs <dir>]"
              << " [--translate] [--timestamps] [--no-context]"
              << " [--single-segment] [--print-progress] [--print-realtime]"
              << " [--print-special] [--offset-ms <n>] [--help]\n\n";

    std::cerr << "Options:\n";
    std::cerr << "  --model,          -m    Path to the STT model/config file\n";
    std::cerr << "  --duration,       -d    Synthetic benchmark audio duration in seconds\n";
    std::cerr << "  --samples,        -s    Exact synthetic input sample count to benchmark\n";
    std::cerr << "  --threads,        -t    Number of runtime threads\n";
    std::cerr << "  --iterations,     -n    Number of measured iterations (default: 5)\n";
    std::cerr << "  --warmup,         -w    Number of warm-up iterations (default: 1)\n";
    std::cerr << "  --language,       -l    Input language code (default: en)\n";
    std::cerr << "  --shared-libs           Directory containing backend shared libraries"
              << " (default: executable directory)\n";
    std::cerr << "  --translate             Translate output to English (default: off)\n";
    std::cerr << "  --timestamps            Include timestamps in transcription (default: off)\n";
    std::cerr << "  --print-special         Include special tokens in output (default: off)\n";
    std::cerr << "  --print-progress        Enable backend progress output (default: off)\n";
    std::cerr << "  --print-realtime        Enable realtime partial output (default: off)\n";
    std::cerr << "  --offset-ms             Initial transcription offset in milliseconds"
              << " (default: 0)\n";
    std::cerr << "  --no-context            Disable cross-segment context reuse (default: off)\n";
    std::cerr << "  --single-segment        Force transcription as a single segment"
              << " (default: off)\n";
    std::cerr << "  --help,           -h    Show this help message and exit\n\n";

    std::cerr << "Examples:\n";
    std::cerr << "  " << program
              << " --model resources_downloaded/models/model.bin"
              << " --samples 160000"
              << " --threads 4\n";
    std::cerr << "  " << program
              << " --model resources_downloaded/models/model.bin"
              << " --duration 10 --threads 4 --iterations 10 --warmup 2\n\n";
}

[[noreturn]] void ReportArgumentErrorAndExit(const char* program, const std::string& message)
{
    LOG_ERROR("%s", message.c_str());
    PrintUsage(program);
    std::exit(1);
}

int ParseIntArgument(const char* program, int& index, int argc, char** argv, const std::string& name)
{
    if (index + 1 >= argc) {
        ReportArgumentErrorAndExit(program, "Missing value for argument: " + name);
    }

    const std::string value(argv[++index]);
    std::size_t consumed = 0;
    const int parsed = std::stoi(value, &consumed, 10);
    if (consumed != value.size()) {
        ReportArgumentErrorAndExit(program, "Invalid integer value for argument: " + name);
    }
    return parsed;
}

double ParseDoubleArgument(const char* program, int& index, int argc, char** argv, const std::string& name)
{
    if (index + 1 >= argc) {
        ReportArgumentErrorAndExit(program, "Missing value for argument: " + name);
    }

    const std::string value(argv[++index]);
    std::size_t consumed = 0;
    const double parsed = std::stod(value, &consumed);
    if (consumed != value.size()) {
        ReportArgumentErrorAndExit(program, "Invalid numeric value for argument: " + name);
    }
    return parsed;
}

std::string ResolveExecutableDirectory(char** argv)
{
    const std::filesystem::path executablePath = std::filesystem::absolute(argv[0]);
    if (!executablePath.parent_path().empty()) {
        return executablePath.parent_path().string();
    }

    return std::filesystem::current_path().string();
}

CliOptions ParseArguments(int argc, char** argv)
{
    CliOptions options;
    options.sharedLibraryPath = ResolveExecutableDirectory(argv);

    if (argc == 1) {
        PrintUsage(argv[0]);
        std::exit(0);
    }

    for (int index = 1; index < argc; ++index) {
        const std::string arg = argv[index];

        if (arg == "--help" || arg == "-h") {
            PrintUsage(argv[0]);
            std::exit(0);
        } else if (arg == "--model" || arg == "-m") {
            if (index + 1 >= argc) {
                ReportArgumentErrorAndExit(argv[0], "Missing value for argument: " + arg);
            }
            options.modelPath = argv[++index];
        } else if (arg == "--duration" || arg == "-d") {
            options.durationSeconds = ParseDoubleArgument(argv[0], index, argc, argv, arg);
        } else if (arg == "--samples" || arg == "-s") {
            const int parsedSamples = ParseIntArgument(argv[0], index, argc, argv, arg);
            if (parsedSamples <= 0) {
                ReportArgumentErrorAndExit(argv[0], "Sample count must be positive.");
            }
            options.targetSamples = static_cast<std::size_t>(parsedSamples);
        } else if (arg == "--threads" || arg == "-t") {
            options.numThreads = ParseIntArgument(argv[0], index, argc, argv, arg);
        } else if (arg == "--iterations" || arg == "-n") {
            options.numIterations = ParseIntArgument(argv[0], index, argc, argv, arg);
        } else if (arg == "--warmup" || arg == "-w") {
            options.numWarmupIterations = ParseIntArgument(argv[0], index, argc, argv, arg);
        } else if (arg == "--language" || arg == "-l") {
            if (index + 1 >= argc) {
                ReportArgumentErrorAndExit(argv[0], "Missing value for argument: " + arg);
            }
            options.language = argv[++index];
        } else if (arg == "--shared-libs") {
            if (index + 1 >= argc) {
                ReportArgumentErrorAndExit(argv[0], "Missing value for argument: " + arg);
            }
            options.sharedLibraryPath = argv[++index];
        } else if (arg == "--offset-ms") {
            options.offsetMs = ParseIntArgument(argv[0], index, argc, argv, arg);
        } else if (arg == "--translate") {
            options.translate = true;
        } else if (arg == "--timestamps") {
            options.timeStamps = true;
        } else if (arg == "--print-special") {
            options.printSpecial = true;
        } else if (arg == "--print-progress") {
            options.printProgress = true;
        } else if (arg == "--print-realtime") {
            options.printRealtime = true;
        } else if (arg == "--no-context") {
            options.noContext = true;
        } else if (arg == "--single-segment") {
            options.singleSegment = true;
        } else {
            ReportArgumentErrorAndExit(argv[0], "Unknown or incomplete argument: " + arg);
        }
    }

    if (options.modelPath.empty()) {
        throw std::invalid_argument("A model path is required.");
    }
    if (options.numThreads <= 0) {
        throw std::invalid_argument("A positive thread count is required.");
    }
    if (options.numIterations <= 0) {
        throw std::invalid_argument("Iterations must be positive.");
    }
    if (options.numWarmupIterations < 0) {
        throw std::invalid_argument("Warm-up iterations cannot be negative.");
    }
    if (options.durationSeconds < 0.0) {
        throw std::invalid_argument("Duration cannot be negative.");
    }
    if (options.durationSeconds > 0.0 && options.targetSamples > 0) {
        throw std::invalid_argument("Use either --duration or --samples, not both.");
    }
    if (options.durationSeconds <= 0.0 && options.targetSamples == 0) {
        throw std::invalid_argument("Provide one of --duration or --samples.");
    }

    return options;
}

} // namespace

int main(int argc, char** argv)
{
    try {
        const CliOptions options = ParseArguments(argc, argv);

        std::size_t targetSamples = options.targetSamples;
        if (targetSamples == 0 && options.durationSeconds > 0.0) {
            targetSamples = static_cast<std::size_t>(
                std::llround(options.durationSeconds * SttBenchmark::GetInputSampleRateHz()));
        }

        SttBenchmarkConfig config;
        config.modelPath = options.modelPath;
        config.sharedLibraryPath = options.sharedLibraryPath;
        config.numSamples = targetSamples;
        config.numThreads = options.numThreads;
        config.numIterations = options.numIterations;
        config.numWarmupIterations = options.numWarmupIterations;
        config.printRealtime = options.printRealtime;
        config.printProgress = options.printProgress;
        config.timeStamps = options.timeStamps;
        config.printSpecial = options.printSpecial;
        config.translate = options.translate;
        config.language = options.language;
        config.offsetMs = options.offsetMs;
        config.noContext = options.noContext;
        config.singleSegment = options.singleSegment;

        SttBenchmark benchmark(std::move(config));
        const int rc = benchmark.Run();
        std::cout << benchmark.GetResults() << std::endl;
        return rc;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n\n";
        PrintUsage(argv[0]);
        return 1;
    }
}
