//
// SPDX-FileCopyrightText: Copyright 2024-2025 Arm Limited and/or its affiliates <open-source-office@arm.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#include <jni.h>
#include "WhisperImpl.hpp"
#include "STT.hpp"

#ifdef __cplusplus
extern "C" {
#endif

// Instantiating a Whisper type STT implementation
static STT<WhisperImpl> stt;

/**
 * Initialize whisper parameters
 *
 * @param env JNI environment
 * @param jprintRealTime  whether to print partial decoding results in real-time
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
Java_com_arm_stt_Whisper_initParams(JNIEnv *env, jobject, jboolean jprintRealtime,
                                    jboolean jprintProgress, jboolean jtimeStamps,
                                    jboolean jprintSpecial, jboolean jtranslate,  jstring jlanguage,
                                    jint jnumThreads, jint joffsetMs, jboolean jnoContext,
                                    jboolean jsingleSegment)
{
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
 * @return context
 */
JNIEXPORT jlong JNICALL Java_com_arm_stt_Whisper_initContext
  (JNIEnv* env, jobject, jstring modelPath)
{
      whisper_context *context = nullptr;
      const char *model_path_chars = env->GetStringUTFChars(modelPath, nullptr);
      context = stt.InitContext<whisper_context>(model_path_chars);
      env->ReleaseStringUTFChars(modelPath, model_path_chars);
      return reinterpret_cast<jlong>(context);
}

/**
 * Free the resources associated with whisper context
 * @param contextPtr pointer to whisper context
 */
JNIEXPORT void JNICALL Java_com_arm_stt_Whisper_freeContext
  (JNIEnv*, jobject, jlong contextPtr)
{
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
  (JNIEnv *env, jobject, jlong contextPtr, jfloatArray audioData)
{
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
