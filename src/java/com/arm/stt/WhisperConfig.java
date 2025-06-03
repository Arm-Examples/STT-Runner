 //
 // SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its affiliates <open-source-office@arm.com>
 //
 // SPDX-License-Identifier: Apache-2.0
 //

package com.arm.stt;

/**
 * @class WhisperConfig
 * @brief Config for setting options for Whisper
 */
public class WhisperConfig {

    private boolean printRealTime;
    private boolean printProgress;
    private boolean timeStamps;
    private boolean printSpecial;
    private boolean translate;
    private String language;
    private Integer numThreads;
    private Integer offsetMs;
    private boolean noContext;
    private boolean singleSegment;

    /**
     * Initializes the Whisper config with the specified settings.
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
    public WhisperConfig(boolean printRealTime, boolean printProgress, boolean timeStamps,
                         boolean printSpecial, boolean translate, String language,
                         int numThreads, int offsetMs, boolean noContext, boolean singleSegment)
    {
        this.printRealTime = printRealTime;
        this.printProgress = printProgress;
        this.timeStamps = timeStamps;
        this.printSpecial = printSpecial;
        this.translate = translate;
        this.language = language;
        this.numThreads = numThreads;
        this.offsetMs = offsetMs;
        this.noContext = noContext;
        this.singleSegment = singleSegment;

    }

    public WhisperConfig() {

    }

     /**
     * Gets the number of threads to use.
     *
     * @return the number of threads
     */
    public Integer getNumThreads() {
        return numThreads;
    }

     /**
     * Sets the number of threads to use
     *
     * @param numThreads the number of threads
     */
    public void setNumThreads(Integer numThreads) {
        this.numThreads = numThreads;
    }

     /**
     * Checks if real-time transcription printing is enabled
     *
     * @return true if enabled, false otherwise
     */
    public boolean isPrintRealTime() {
        return printRealTime;
    }


     /**
     * Enables or disables real-time transcription printing
     *
     * @param printRealTime true to enable, false to disable
     */
    public void setPrintRealTime(boolean printRealTime) {
        this.printRealTime = printRealTime;
    }


     /**
     * Checks if progress printing is enabled
     *
     * @return true if enabled, false otherwise
     */
    public boolean isPrintProgress() {
        return printProgress;
    }

     /**
     * Enables or disables progress printing
     *
     * @param printProgress true to enable, false to disable
     */
    public void setPrintProgress(boolean printProgress) {
        this.printProgress = printProgress;
    }

     /**
     * Checks if timestamps are included in the transcription
     *
     * @return true if included, false otherwise
     */
    public boolean isTimeStamps() {
        return timeStamps;
    }


     /**
     * Enables or disables inclusion of timestamps
     *
     * @param timeStamps true to include, false to exclude
     */
    public void setTimeStamps(boolean timeStamps) {
        this.timeStamps = timeStamps;
    }

     /**
     * Checks if special characters are printed in the output
     *
     * @return true if printed, false otherwise
     */
    public boolean isPrintSpecial() {
        return printSpecial;
    }


     /**
     * Enables or disables printing of special characters
     *
     * @param printSpecial true to enable, false to disable
     */
    public void setPrintSpecial(boolean printSpecial) {
        this.printSpecial = printSpecial;
    }

     /**
     * Checks if translation to English is enabled
     *
     * @return true if enabled, false otherwise
     */
    public boolean isTranslate() {
        return translate;
    }

     /**
     * Enables or disables translation to English
     *
     * @param translate true to enable, false to disable
     */
    public void setTranslate(boolean translate) {
        this.translate = translate;
    }

     /**
     * Gets the language code for transcription
     *
     * @return the language code
     */
    public String getLanguage() {
        return language;
    }

     /**
     * Sets the language code for transcription
     *
     * @param language the language code (e.g., "en", "es")
     */
    public void setLanguage(String language) {
        this.language = language;
    }

     /**
     * Gets the offset in milliseconds to start processing from
     *
     * @return the offset in milliseconds
     */
    public Integer getOffsetMs() {
        return offsetMs;
    }

     /**
     * Sets the offset in milliseconds to start processing from.
     *
     * @param offsetMs the offset in milliseconds
     */
    public void setOffsetMs(Integer offsetMs) {
        this.offsetMs = offsetMs;
    }

     /**
     * Checks if context from previous segments is disabled.
     *
     * @return true if context is disabled, false otherwise
     */
    public boolean isNoContext() {
        return noContext;
    }

     /**
     * Enables or disables context from previous segments.
     *
     * @param noContext true to disable, false to enable
     */
    public void setNoContext(boolean noContext) {
        this.noContext = noContext;
    }

     /**
     * Checks if only a single segment should be returned.
     *
     * @return true if only one segment is returned, false otherwise
     */
    public boolean isSingleSegment() {
        return singleSegment;
    }

     /**
     * Enables or disables returning only a single segment.
     *
     * @param singleSegment true to enable, false to disable
     */
    public void setSingleSegment(boolean singleSegment) {
        this.singleSegment = singleSegment;
    }
}
