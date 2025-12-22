#!/usr/bin/env python
"""
Produce a quantized / normalized checksum for the input wav file.

Takes a file parameter and outputs the checksum to stdout or an error.
"""

import argparse
import hashlib
import pathlib
import warnings

import numpy as np
from scipy.io import wavfile


def checksum(filepath: pathlib.Path, precision: int = 6) -> str:
    warnings.filterwarnings("ignore", category=wavfile.WavFileWarning)
    sr, data = wavfile.read(filepath)

    if sr != 48000:
        raise ValueError("sr should be 48k")

    if data.dtype != np.float32:
        raise ValueError("input wav for checksum must be float 32")

    quantized = np.around(data, decimals=precision)
    hash = hashlib.md5(quantized.tobytes())
    return hash.hexdigest()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Checksum from a 32bit wav")
    parser.add_argument(
        "-f",
        "--file",
        type=pathlib.Path,
        required=True,
        help="Relative path to the wav file.",
    )

    args = parser.parse_args()

    out = checksum(args.file)
    print(out)
