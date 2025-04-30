//
// SPDX-FileCopyrightText: Copyright 2024-2025 Arm Limited and/or its affiliates <open-source-office@arm.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#define CATCH_CONFIG_MAIN

#include <catch.hpp>
#include "WhisperImpl.hpp"
#include <list>
#include <sstream>

std::string testModelsDir = TEST_MODELS_DIR;
std::string testDataDir = TEST_DATA_DIR;
std::string modelPath =  testModelsDir + "/model.bin";
std::string audioDataPath =  testDataDir + "/audioData.csv";

std::vector<float> ReadAudioData(const std::string& pathToAudioData)
{
    std::ifstream inputFile(pathToAudioData);
    std::stringstream ss;
    ss << inputFile.rdbuf();
    std::vector<float> values;
    for (float i; ss >> i;) {
        values.push_back(i);
        if (ss.peek() == ',' || ss.peek() == ' ')
            ss.ignore();
    }
    return values;
}

/**
 * Test transcription of jfk audio file
 */
TEST_CASE("Test audio file float representation to text")
{
    STT<WhisperImpl> stt;
    auto* context = stt.InitContext<whisper_context>(modelPath.c_str());
    std::vector<float> audioData = ReadAudioData(audioDataPath);

    const bool printRealtime = true;
    const bool printProgress = false;
    const bool timeStamps = true;
    const bool printSpecial = false;
    const bool translate = false;
    const char *language = "en";
    const int numThreads = 2;
    const int offsetMs = 0;
    const bool noContext = true;
    const bool singleSegment = false;

    stt.InitParams(printRealtime, printProgress, timeStamps, printSpecial, translate, language,
                   numThreads, offsetMs, noContext, singleSegment);

    const std::string transcribed = stt.FullTranscribe<whisper_context>(context, &audioData[0], audioData.size());

    CHECK(transcribed == " And so my fellow Americans, ask not what your country can do for you, ask what you can do for your country.");
}




