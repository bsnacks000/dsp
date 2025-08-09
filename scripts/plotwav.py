#!/usr/bin/env python3
"""
Plotting resources.

"""

# import sys

# from matplotlib import pyplot as plt

# if __name__ == "__main__":
#     x = []
#     for line in sys.stdin:
#         x.append(float(line))
#     print(len(x))
#     plt.plot(x)
#     plt.show()

import argparse

import matplotlib.pyplot as plt
import numpy as np
from scipy.io import wavfile


def normalize_audio(data):
    """Normalize int PCM data to float32 in range [-1, 1]."""
    if data.dtype == np.int16:
        return data.astype(np.float32) / 32768.0
    elif data.dtype == np.int32:
        return data.astype(np.float32) / 2147483648.0
    elif data.dtype == np.uint8:
        return (data.astype(np.float32) - 128) / 128.0
    elif data.dtype == np.float32 or data.dtype == np.float64:
        return data.astype(np.float32)
    else:
        raise ValueError(f"Unsupported audio dtype: {data.dtype}")


def main():
    parser = argparse.ArgumentParser(description="Plot a WAV file")
    parser.add_argument("-i", "--input", required=True, help="Path to input WAV file")
    args = parser.parse_args()

    # Read WAV file
    samplerate, data = wavfile.read(args.input)

    # Handle stereo files by selecting one channel
    if data.ndim > 1:
        data = data[:, 0]

    data = normalize_audio(data)
    # Time axis for plotting
    # duration = data.shape[0] / samplerate
    # time = np.linspace(0, duration, num=data.shape[0])

    # Plot
    plt.figure(figsize=(10, 4))
    plt.plot(data, linewidth=0.5)
    plt.xlabel("Time [s]")
    plt.ylabel("Amplitude")
    plt.title(f"Waveform: {args.input}")
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    main()
