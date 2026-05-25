"""
This is the basic script for creating interpolated decks.

memory:
    a - original deck (M, N)
    b - a.T           (N, M)

    c - interpolated transpose (N, K)
    d - c.T          (K,N)

M is the original number of rows (4)
N is the original number of cols (3)
K is the target interpolation (16)

transpose -> interp -> transpose

We need space for four matrices.

"""

from pprint import pprint

a = [
    [1.0, 2.0, 3.0],
    [4.0, 5.0, 6.0],
    [7.0, 8.0, 9.0],
    [10.0, 11.0, 12.0],
]

b = [
    [0.0, 0.0, 0.0, 0.0],
    [0.0, 0.0, 0.0, 0.0],
    [0.0, 0.0, 0.0, 0.0],
]


def transpose(b: list[list[float]], a: list[list[float]]):
    for i in range(len(a)):
        for j in range(len(b)):
            b[j][i] = a[i][j]


def npinterp(out: list[float], in_: list[float]):
    dx = 1.0 / (len(in_) - 1)
    dt = 1.0 / (len(out) - 1)

    for i in range(len(out)):
        if i == len(out) - 1:
            out[i] = in_[len(in_) - 1]
        else:
            t = i * dt
            # print(t)
            if t <= 0:
                out[i] = in_[0]
                continue
            elif t >= 1.0:
                out[i] = in_[len(in_) - 1]
                continue
            else:
                pos = t / dx
                idx = int(pos)
                frac = pos - idx

                out[i] = round((1.0 - frac) * in_[idx] + frac * in_[idx + 1], 4)


# b <- a.T
transpose(b, a)
print("b:")
pprint(b)

# alloc c (K, N)
c = [[0.0 for _ in range(16)] for _ in range(3)]

# fill c with interpolated
for i in range(len(b)):
    npinterp(c[i], b[i])

print("c:")
pprint(c)
# alloc d (final)
d = [[0.0 for _ in range(3)] for _ in range(16)]

# d <- c.T
transpose(d, c)
print("d:")
pprint(d)
len(d)
