//
// SPDX-FileCopyrightText: Copyright 2024-2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef STT_STT_HPP
#define STT_STT_HPP

#include <string>

#if defined(ENABLE_STREAMLINE)
#include "profiling/StreamlineStt.hpp"
#endif

/**
 * Interface to STT(Speech to text)
 * Contains generic STT functions
 * Should invoke whatever implementation this is initialised with
 */
template<typename T>
class STT {
private:
    T stt;
public:
    STT() = default;

    /**
     * Initializes the Whisper parameters with the specified settings.
     * @param printRealTime  whether to print partial decoding results in real-time
     * @param printProgress  whether to print progress information
     * @param timeStamps     whether to include timestamps in the transcription
     * @param printSpecial   whether to include special tokens (e.g., markers) in the output
     * @param translate      whether to translate the transcription to English
     * @param language       the language code for transcription (e.g., "en", "fr", etc.)
     * @param numThreads     the number of CPU threads to use for transcription
     * @param offsetMs       an initial time offset (in milliseconds) for the transcription
     * @param noContext      whether to disable reusing context between segments
     * @param singleSegment  whether to transcribe the entire audio in a single segment
     */
    void InitParams(const bool printRealTime, const bool printProgress, const bool timeStamps,
                    const bool printSpecial, const bool translate, const char *language,
                    const int numThreads, const int offsetMs, const bool noContext,
                    const bool singleSegment)
    {
#if defined(ENABLE_STREAMLINE)
        sl::InitThreadOnce();
        sl::Scope scope(sl::CH_INIT, ANNOTATE_BLUE, "STT::InitParams");
        if (translate) {
            sl::marker(ANNOTATE_CYAN, "Translate enabled");
        }
        if (singleSegment) {
            sl::marker(ANNOTATE_YELLOW, "Single segment mode");
        }
#endif
        stt.InitParams(printRealTime, printProgress, timeStamps, printSpecial, translate,
                       language, numThreads, offsetMs, noContext, singleSegment);
    }

    /**
     * Function to load the chosen STT model and to init the context
     * @tparam P stt context type
     * @param pathToModel path to the model location
     * @param sharedLibraryPath path to location of shared libs
     * @return stt context pointer
     */
    template<typename P>
    P *InitContext(const char *pathToModel, const char *sharedLibraryPath )
    {
#if defined(ENABLE_STREAMLINE)
        sl::InitThreadOnce();
        sl::Scope scope(sl::CH_INIT, ANNOTATE_BLUE, "STT::InitContext");
        sl::marker(ANNOTATE_BLUE, "Loading model");
#endif
        return stt.InitContext(pathToModel, sharedLibraryPath);
    }

    /**
     * Function to free the previously initialised stt context
     * @tparam P stt context type
     * @param contextPtr stt context pointer
     */
    template<typename P>
    void FreeContext(P* contextPtr)
    {
#if defined(ENABLE_STREAMLINE)
        sl::InitThreadOnce();
        sl::Scope scope(sl::CH_CONTROL, ANNOTATE_DKGRAY, "STT::FreeContext");
#endif
        stt.FreeContext(contextPtr);
    }

    /**
     * The entire transcription inference loop
     * @tparam P stt context type
     * @param contextPtr stt context pointer
     * @param audioData  audio data to transcribe
     * @param audioDataLength length of the Audio data supplied
     * @return String containing the transcribed text.
     */
    template<typename P>
    std::string FullTranscribe(P* contextPtr, float* audioData, int audioDataLength)
    {
#if defined(ENABLE_STREAMLINE)
        sl::InitThreadOnce();
        sl::Scope scope(sl::CH_TRANSCRIBE, ANNOTATE_GREEN, "STT::FullTranscribe");
        sl::marker(ANNOTATE_GREEN, "Transcription start");
#endif
        std::string transcribed = stt.FullTranscribe(contextPtr, audioData, audioDataLength);
#if defined(ENABLE_STREAMLINE)
        sl::marker(ANNOTATE_GREEN, "Transcription complete");
#endif
        return transcribed;
    }
};
#endif //STT_STT_HPP
