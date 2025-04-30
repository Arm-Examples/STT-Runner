//
// SPDX-FileCopyrightText: Copyright 2024-2025 Arm Limited and/or its affiliates <open-source-office@arm.com>
//
// SPDX-License-Identifier: Apache-2.0
//

package com.arm.stt;

import static org.junit.Assert.assertTrue;
import org.junit.Test;
import java.util.Scanner;
import java.io.*;
import java.util.*;

/**
 * The WhisperTestApp is used to run a simple speech to text execution using the native Java
 * functions described in Whisper.java, using a known audio input.
 */
public class WhisperTestApp {

    @Test
    public void testInference() {
        Whisper whisper = new Whisper();
        String modelPath = System.getProperty("model_dir");
        String testDataPath = System.getProperty("test_data_dir");
        long context = whisper.initContext(modelPath + "/model.bin");
        float[] audioData = readCSV(testDataPath + "/audioData.csv");

        boolean printRealtime=true;
        boolean printProgress=false;
        boolean timeStamps=true;
        boolean printSpecial=false;
        boolean translate=false;
        String language="en";
        int numThreads=2;
        int offsetMs=0;
        boolean noContext=true;
        boolean singleSegment=false;

        WhisperConfig whisperConfig = new WhisperConfig(printRealtime, printProgress, timeStamps,
                                                        printSpecial, translate, language,
                                                        numThreads, offsetMs, noContext,
                                                        singleSegment);

        whisper.initParameters(whisperConfig);
        String transcribed = whisper.fullTranscribe(context, audioData);

        String expected = " And so my fellow Americans, ask not what your country can do for you, ask what you can do for your country.";
        assertTrue("Expected: [" + expected + "] but was [" + transcribed + "]",
                   transcribed.equals(expected));
    }

    /**
     * Brute force test function to read as CSV into a float Array
     *
     * @param pathToCSVFile
     * @return
     */
    private float[] readCSV(String pathToCSVFile) {

        File file = new File(pathToCSVFile);

        ArrayList<Float> floatList = new ArrayList();
        try {
            Scanner inputStream = new Scanner(file);
            int i = 0;
            while (inputStream.hasNextLine()) {
                String line = inputStream.nextLine();
                String[] values = line.split(",");

                for (String value : values) {
                    floatList.add(Float.parseFloat(value.trim()));
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        // Convert List<Float> to float[]
        float[] floatArray = new float[floatList.size()];
        for (int i = 0; i < floatList.size(); i++) {
            floatArray[i] = floatList.get(i);
        }

        return floatArray;
    }
}
