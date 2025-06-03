//
// SPDX-FileCopyrightText: Copyright 2024-2025 Arm Limited and/or its affiliates <open-source-office@arm.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "whisper.h"
#include "STT.hpp"
#include <string>

#ifndef STT_WHISPER_IMPL_HPP
#define STT_WHISPER_IMPL_HPP

/**
* @brief Whisper Implementation of our STT API
*
*/
class WhisperImpl {
private:

    std::string strLang{"en"};
    struct whisper_full_params whisperParams{};

    /**
     * Function to retrieve the total number of text segments
     * @param contextPtr whisper_context pointer
     * @return segment count
     */
    int GetTextSegmentCount(whisper_context* contextPtr)
    {
        return whisper_full_n_segments(contextPtr);
    }

    /**
     * Function to retrieve the text segment at a given index
     * @param contextPtr whisper_context pointer
     * @param index index of the text segment
     * @return text segment
     */
    const char * GetTextSegment(whisper_context* contextPtr, int index)
    {
        const char *text = whisper_full_get_segment_text(contextPtr, index);
        return text;
    }

public:
    WhisperImpl() = default;

    /**
    * Initializes the Whisper parameters with the specified settings.
    * @param printRealTime  whether to print partial decoding results in real-time
    * @param printProgress  whether to print progress information
    * @param printTimestamps whether to include timestamps in the transcription
    * @param printSpecial   whether to include special tokens (e.g., markers) in the output
    * @param translate      whether to translate the transcription to English
    * @param language       the language code for transcription (e.g., "en", "fr", etc.)
    * @param numThreads     the number of CPU threads to use for transcription
    * @param offsetMs       an initial time offset (in milliseconds) for the transcription
    * @param noContext      whether to disable reusing context between segments
    * @param singleSegment  whether to transcribe the entire audio in a single segment
    */
    void InitParams(const bool printRealTime, const bool printProgress, const bool printTimestamps,
                    const bool printSpecial, const bool translate, const char *language,
                    const int numThreads, const int offsetMs, const bool noContext,
                    const bool singleSegment)
    {
        this->strLang = std::string(language);
        this->whisperParams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
        this->whisperParams.print_realtime   = printRealTime;
        this->whisperParams.print_progress   = printProgress;
        this->whisperParams.print_timestamps = printTimestamps;
        this->whisperParams.print_special    = printSpecial;
        this->whisperParams.translate        = translate;
        this->whisperParams.language         = strLang.c_str();
        this->whisperParams.n_threads        = numThreads;
        this->whisperParams.offset_ms        = offsetMs;
        this->whisperParams.no_context       = noContext;
        this->whisperParams.single_segment   = singleSegment;

    }

    /**
    * Function to load the chosen STT model and to init the context
    * @param pathToModel path to the model location
    * @return whisper_context pointer
    */
    whisper_context* InitContext(const char *pathToModel)
    {
        whisper_context *context = whisper_init_from_file_with_params(pathToModel, whisper_context_default_params());
        return context;
    }

    /**
     * Function to free the previously initialised whisper_context
     * @param contextPtr whisper_context pointer
     */
    void FreeContext(whisper_context* contextPtr)
    {
        whisper_free(contextPtr);
    }

    /**
    * The full transcription inference loop, and retrieval of all text segments
    * Taken from whisper.cpp/examples/whisper.android/lib/src/main/jni/whisper/jni.c and slightly
    * modified
    * @param contextPtr whisper_context pointer
    * @param audioDataPtr  pointer to audio data to transcribe
    * @param audioDataLength length of the audio data array
    * @return String containing the transcribed text
    */
    std::string FullTranscribe(whisper_context* contextPtr, const float* audioDataPtr,
                               const int audioDataLength)
    {

        whisper_full(contextPtr, whisperParams, &audioDataPtr[0], audioDataLength);
        whisper_reset_timings(contextPtr);
        whisper_print_timings(contextPtr);

        int count = GetTextSegmentCount(contextPtr);

        std::string transcribed;
        for(int i = 0; i < count; i++)
        {
            transcribed += GetTextSegment(contextPtr, i);
        }
        return transcribed;
    }
};

#endif //STT_WHISPER_IMPL_HPP
