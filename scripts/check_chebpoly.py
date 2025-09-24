import matplotlib.pyplot as plt
import numpy as np

# this satisfies the recurrence relation for chebyshev polynomials..
# This is defined in Dodge/Jerse p.148
# F(x)=h0/2 * T0(x) + h1T1(x) + h2T2(x) + ... + h[N]T[N](x)
# Where T is the partial defined by the chebyshev recurrence relation
# T[n+1](x)=2xT[n](x) − T[n−1](x)


def chebpoly(x: float, h: list[float]) -> float:
    n = len(h) - 1
    s = 0.5 * h[0]  # handle dc T0
    if n == 0:
        return s
    t_nm1 = 1.0  # T0

    t_n = x  # T1
    s += h[1] * t_n  # scale T1

    for k in range(2, n + 1):
        tk = 2 * x * t_n - t_nm1
        s += h[k] * tk  # apply coeff
        t_nm1, t_n = t_n, tk  # recurrence
    return s


# These are from gen13 docs for csound https://csound.com/docs/manual/GEN13.html
# square / saw / inharmonic spectra
#
# NOTE that adding even partials will create asymmetry
# harmonic weights are scaled so we're not limited to -1, 1

# h = [0, 0, 0, 1, 0]  # show just the T3 partial at full strength
# h = [0, 100, 0, -33, 0, 20, 0, -14.2, 0, 11.1, 0, -9.09, 0, 7.69]
# h = [0, 100, -50, -33, 25, 20, -16.7, -14.2, 12.5, 11.1, -10, -9.09, 8.333, 7.69, -7.14]
h = [0, 0, 0, -0.1, 0, 0.3, 0, -0.5, 0, 0.7, 0, -0.9, 0, 1, 0, -1, 0]

xs = np.linspace(-1, 1, 1024)
buf = np.array([chebpoly(x, h) for x in xs])
buf /= np.max(np.abs(buf))  # critical normalize table

plt.plot(xs, buf)
plt.grid(True)
plt.show()
