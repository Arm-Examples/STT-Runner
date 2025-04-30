//
// SPDX-FileCopyrightText: Copyright 2024-2025 Arm Limited and/or its affiliates <open-source-office@arm.com>
//
// SPDX-License-Identifier: Apache-2.0
//

package com.arm.stt;

/**
 * The Whisper class is used to access the underlying C++ implementation
 * of the Whisper API via the JNI.
 */
public class Whisper {
    // Used to load the 'STT_lib' library on application startup.
    static {
        System.loadLibrary("arm-stt-jni");
    }

    /**
    * Function to load the chosen Whisper model and to init the context
    *
    * @param  modelPath  path to the file on disk
    * @return pointer to the context object
    */
    public native long initContext(String modelPath);

    /**
     * Function to extracts parameters from WhisperConfig object and
     * run the private InitParams function to initialize the parameters
     *
     * @param whisperConfig the configuration object containing Whisper parameter settings
     */
    public void initParameters(WhisperConfig whisperConfig)
    {
        boolean printRealTime = whisperConfig.isPrintRealTime();
        boolean printProgress = whisperConfig.isPrintProgress();
        boolean timeStamps = whisperConfig.isTimeStamps();
        boolean printSpecial = whisperConfig.isPrintSpecial();
        boolean translate = whisperConfig.isTranslate();
        String language = whisperConfig.getLanguage();
        int numThreads = whisperConfig.getNumThreads();
        int offsetMs = whisperConfig.getOffsetMs();
        boolean noContext = whisperConfig.isNoContext();
        boolean singleSegment =whisperConfig.isSingleSegment();

        initParams(printRealTime, printProgress, timeStamps, printSpecial, translate, language,
                numThreads, offsetMs, noContext, singleSegment);
    }

    /**
     * Initializes the native Whisper parameters with the specified settings.
     *
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
    private native void initParams(boolean printRealTime, boolean printProgress, boolean timeStamps,
                                  boolean printSpecial, boolean translate, String language,
                                  int numThreads, int offsetMs, boolean noContext,
                                  boolean singleSegment);

    /**
    * Function to free the previously initialised whisper_context
    *
    * @param  contextPtr  pointer to the context object previously initialised
    */
    public native void freeContext(long contextPtr);

    /**
    * Function to run the entire transcription inference loop
    *
    * @param  contextPtr pointer to the context object previously initialised
    * @param  audioData  audio data to transcribe
    * @return transcribed string object
    */
    public native String fullTranscribe(long contextPtr, float[] audioData);
}
