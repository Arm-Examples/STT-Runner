//
// SPDX-FileCopyrightText: Copyright 2024-2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#include <jni.h>
#include "WhisperImpl.hpp"
#include "STT.hpp"

#if defined(ENABLE_STREAMLINE)
#include "profiling/StreamlineStt.hpp"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Instantiating a Whisper type STT implementation
static STT<WhisperImpl> stt;

/**
 * Initialize whisper parameters
 *
 * @param env JNI environment
 * @param jprintRealtime  whether to print partial decoding results in real-time
 * @param jprintProgress  whether to print progress information
 * @param jtimeStamps     whether to include timestamps in the transcription
 * @param jprintSpecial   whether to include special tokens (e.g., markers) in the output
 * @param jtranslate      whether to translate the transcription to English
 * @param jlanguage       the language code for transcription (e.g., "en", "fr", etc.)
 * @param jnumThreads     the number of CPU threads to use for transcription
 * @param joffsetMs       an initial time offset (in milliseconds) for the transcription
 * @param jnoContext      whether to disable reusing context between segments
 * @param jsingleSegment  whether to transcribe the entire audio in a single segment
 */
JNIEXPORT void JNICALL
Java_com_arm_stt_Whisper_initParams(JNIEnv *env, jobject, const jboolean jprintRealtime,
                                    const jboolean jprintProgress, const jboolean jtimeStamps,
                                    const jboolean jprintSpecial, const jboolean jtranslate, jstring jlanguage,
                                    const jint jnumThreads, const jint joffsetMs, const jboolean jnoContext,
                                    const jboolean jsingleSegment)
{
#if defined(ENABLE_STREAMLINE)
    sl::InitThreadOnce();
    sl::Scope scope(sl::CH_CONTROL, ANNOTATE_DKGRAY, "JNI::Whisper_initParams");
#endif
    const char *language_chars = env->GetStringUTFChars(jlanguage, nullptr);
    stt.InitParams(jprintRealtime, jprintProgress, jtimeStamps, jprintSpecial,
                                                 jtranslate, language_chars, jnumThreads, joffsetMs,
                                                 jnoContext, jsingleSegment);
    env->ReleaseStringUTFChars(jlanguage, language_chars);
}

/**
 * Initialize whisper context
 *
 * @param env JNI environment
 * @param modelPath path to the model file
 * @param sharedLibraryPath path to the shared libraries
 * @return context
 */
JNIEXPORT jlong JNICALL Java_com_arm_stt_Whisper_initContext
  (JNIEnv* env, jobject, jstring modelPath, jstring sharedLibraryPath)
{
#if defined(ENABLE_STREAMLINE)
    sl::InitThreadOnce();
    sl::Scope scope(sl::CH_CONTROL, ANNOTATE_DKGRAY, "JNI::Whisper_initContext");
#endif
    const auto sharedLibraryPathNative = env->GetStringUTFChars(sharedLibraryPath, nullptr);
    const auto modelPathChars = env->GetStringUTFChars(modelPath, nullptr);
    whisper_context *context = stt.InitContext<whisper_context>(modelPathChars, sharedLibraryPathNative);
    env->ReleaseStringUTFChars(modelPath, modelPathChars);
    env->ReleaseStringUTFChars(sharedLibraryPath, sharedLibraryPathNative);
    return reinterpret_cast<jlong>(context);
}

/**
 * Free the resources associated with whisper context
 * @param contextPtr pointer to whisper context
 */
JNIEXPORT void JNICALL Java_com_arm_stt_Whisper_freeContext
  (JNIEnv*, jobject, const jlong contextPtr)
{
#if defined(ENABLE_STREAMLINE)
    sl::InitThreadOnce();
    sl::Scope scope(sl::CH_CONTROL, ANNOTATE_DKGRAY, "JNI::Whisper_freeContext");
#endif
    auto *context = reinterpret_cast<struct whisper_context *>(contextPtr);
    stt.FreeContext<whisper_context>(context);
}

/**
 * Full transcribe function to
 * @param env JNI environment
 * @param contextPtr pointer to whisper context
 * @param audioData audio data to transcribe
 * @return full transcription
 */
JNIEXPORT jstring JNICALL Java_com_arm_stt_Whisper_fullTranscribe
  (JNIEnv *env, jobject, const jlong contextPtr, jfloatArray audioData)
{
#if defined(ENABLE_STREAMLINE)
    sl::InitThreadOnce();
    sl::Scope scope(sl::CH_CONTROL, ANNOTATE_DKGRAY, "JNI::Whisper_fullTranscribe");
#endif
    auto *context = reinterpret_cast<struct whisper_context *>(contextPtr);
    jfloat *audio_data_arr = env->GetFloatArrayElements(audioData, nullptr);
    const jsize audio_data_length = env->GetArrayLength(audioData);

    const std::string transcribed = stt.FullTranscribe(context, audio_data_arr, audio_data_length);

    env->ReleaseFloatArrayElements(audioData, audio_data_arr, JNI_ABORT);
    return env->NewStringUTF(transcribed.c_str());
}

#ifdef __cplusplus
}
#endif
