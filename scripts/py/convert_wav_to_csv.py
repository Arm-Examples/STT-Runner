#
# SPDX-FileCopyrightText: Copyright 2025-2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
#
# SPDX-License-Identifier: Apache-2.0
#

"""
Example use:
python convert_wav_to_csv.py --wav_file_path path/to/wav_file.wav --sample_rate 48000 --output_file_path audioData.csv
"""

import os
import sys

import argparse
import librosa
import numpy as np


def read_wav_to_array(filepath, sample_rate=16000):
    """
    Read the wav file from filepath and convert to an array

    @param filepath: Path to the WAV file
    @param sampling_rate: desired sampling rate, defaults to 16000
    """

    audio_data, sr = librosa.load(filepath, sr=sample_rate, mono=True)
    audio_data_rounded = np.round(audio_data.astype(np.float64), 8)

    flattened = audio_data_rounded.flatten()
    return flattened


def main(args):

    """
    Main function for converting wav to csv
    """

    file_exists = os.path.isfile(args.wav_file_path)
    if not file_exists:
        print("Error, supplied WAV file path does not exist", file=sys.stderr)
        sys.exit(1)

    try:
        wav_as_array = read_wav_to_array(args.wav_file_path, args.sample_rate)
        np.savetxt(args.output_file_path, wav_as_array, fmt="%.8f,", newline='')
    except Exception as e:
        print(f"Error converting WAV to CSV: {e}", file=sys.stderr)
        sys.exit(1)


parser = argparse.ArgumentParser()
parser.add_argument("--wav_file_path", type=str, help="WAV file to convert to csv",
                    required=True)
parser.add_argument("--output_file_path", type=str, help="Path for generated data",
                    required=True)
parser.add_argument("--sample_rate", type=int,
                    help="Sample rate we want the wav file resampled to.", default=16000)
ARGS = parser.parse_args()

if __name__ == "__main__":
    main(ARGS)
