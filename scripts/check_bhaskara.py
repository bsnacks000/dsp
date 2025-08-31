#!/usr/bin/env python3
#!/usr/bin/env python3
"""
Plotting resources.

"""

import math

import matplotlib.pyplot as plt
import numpy as np

# NOTE: these functions assume x(0,1)

# ref: https://en.wikipedia.org/wiki/Bh%C4%81skara_I%27s_sine_approximation_formula
# https://gist.github.com/MrAnnix/ecb55cec0b9da5c34988ae79d008f4a2


# bhaskara fast quarter sine and cos approx
def fast_qsinf(x: float) -> float:
    return (16 * x * (math.pi - x)) / (5 * math.pi**2 - 4 * x * (math.pi - x))


def fast_qcosf(x: float) -> float:
    return fast_qsinf((math.pi * 0.5) - x)


# faster still but poor slope
# def cubic_sin(x: float) -> float:
#     return x * (1.0 - 0.1666666 * x**2)


# full sine branchless (in C with ?) waveshaper
def fast_sinf(x: float) -> float:
    quad = x * 4.0  # 0..4
    q = int(quad)  # 0-4
    qpos = quad - q  # NOTE: flipping this makes for interesting phase distortion

    # flip value vertically if q is odd .. 2nd quarter
    qpos = 1.0 - qpos if (q & 1) else qpos

    # calculate the angle based on the qpos
    angle = qpos * math.pi * 0.5

    # check even and flip sign on second half .. reflect
    sign = -1.0 if q & 2 else 1.0

    return sign * fast_qsinf(angle)


def fast_cosf(x: float) -> float:
    return fast_sinf(x + 0.25)


# half sine branchless
def fast_hsinf(x: float) -> float:
    d = x * 2.0
    q = int(d)
    qpos = d - q  # NOTE: flipping this makes for interesting phase distortion

    # flip value if q is odd .. fill second quarter
    qpos = 1.0 - qpos if (q & 1) else qpos

    angle = qpos * math.pi * 0.5
    return fast_qsinf(angle)


# probably less useful but for completedness
def fast_hcosf(x: float) -> float:
    return fast_hsinf(x + 0.5)


def main():
    fs = (fast_qsinf, fast_qcosf, fast_hsinf, fast_hcosf, fast_sinf, fast_cosf)
    line = np.linspace(0.0, 1.0, 1024)

    for i, f in enumerate(fs):
        if i in (0, 1):
            # for qsinf we need to shave off the input signal by 1/pi
            data = [f(math.pi * i * 0.5) for i in line]

        else:  # cycle quads computed internally so pass the full phase value.
            data = [f(i) for i in line]

        # Plot
        plt.figure(figsize=(10, 4))
        plt.plot(data, linewidth=0.5)
        plt.xlabel("Time [s]")
        plt.ylabel("Amplitude")
        plt.title("Waveform")
        plt.tight_layout()
        plt.show()


if __name__ == "__main__":
    main()
