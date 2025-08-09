#!/usr/bin/env python3
"""
Plotting resources.
"""

import sys

from matplotlib import pyplot as plt

if __name__ == "__main__":
    x = []
    for line in sys.stdin:
        x.append(float(line))
    print(len(x))
    plt.plot(x)
    plt.show()
