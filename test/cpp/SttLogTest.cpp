//
// SPDX-FileCopyrightText: Copyright 2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#include <catch.hpp>
#include "WhisperImpl.hpp"
#include "SttErrorMessages.hpp"
#include "Logger.hpp"
#include <vector>
#include <string>

namespace {

const std::string testModelsDir = TEST_MODELS_DIR;
const std::string backendSharedLibDir = BACKEND_SHARED_LIB_DIR;

bool contains(const std::string& str, const std::string& sub) {
    return str.find(sub) != std::string::npos;
}

} // namespace

/**
 * Tests for error handling and logging in WhisperImpl
 */
TEST_CASE("WhisperImpl error logging") {
    STT<WhisperImpl> stt;

    SECTION("InitContext throws for missing model path") {
        const std::string invalidModelPath = testModelsDir + "/non_existent_model.bin";
        try {
            stt.InitContext<whisper_context>(invalidModelPath.c_str(), backendSharedLibDir.c_str());
            FAIL("Expected InitContext to throw for invalid model path");
        } catch (const std::runtime_error& e) {
            auto expectedMsg = SttLog::vformat(SttErrorMessages::INIT_CONTEXT_FAIL,
                                                invalidModelPath.c_str());
            CHECK(contains(e.what(), expectedMsg));
        }
    }

    SECTION("FullTranscribe throws for null context") {
        std::vector<float> mockAudio{0.0f, 1.0f};
        try {
            stt.FullTranscribe<whisper_context>(nullptr,
                                                mockAudio.data(),
                                                static_cast<int>(mockAudio.size()));
            FAIL("Expected FullTranscribe to throw for null context");
        } catch (const std::invalid_argument& e) {
            CHECK(contains(e.what(), SttErrorMessages::FULL_TRANSCRIBE_NULL_CTX));
        }
    }

    SECTION("FullTranscribe throws for invalid audio buffer") {
        // Use a non-null mock pointer to bypass the context null guard; the check happens before the context is used.
        auto* mockContext = reinterpret_cast<whisper_context*>(0x1);
        try {
            stt.FullTranscribe<whisper_context>(mockContext, nullptr, 0);
            FAIL("Expected FullTranscribe to throw for invalid audio buffer");
        } catch (const std::invalid_argument& e) {
            auto expectedMsg = SttLog::vformat(SttErrorMessages::FULL_TRANSCRIBE_INVALID_AUDIO,
                                               (const void*)nullptr,
                                               0);
            CHECK(contains(e.what(), expectedMsg));
        }
    }
}
