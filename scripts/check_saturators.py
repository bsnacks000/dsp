import matplotlib.pyplot as plt
import numpy as np


# --- Reference functions ---
def hyptan_saturator(x, amt):
    return np.tanh(amt * x) / np.tanh(amt)


def arctan_saturator(x, amt):
    return np.arctan(amt * x) / np.arctan(amt)


def soft_clip_reiss(x, amt):
    return np.sign(x) * (1.0 - np.exp(-np.abs(amt * x)))


# --- Fast approximations ---
def fast_tanh(x):
    xx = x * x
    return x * (27.0 + xx) / (27.0 + 9.0 * xx)


def softsign(x):
    return x / (1.0 + np.abs(x))


def fast_atan(x):
    return np.pi * 0.25 * softsign(x)


def hyptan_saturator_fast(x, amt):
    return fast_tanh(amt * x) / fast_tanh(amt)


def softsign_saturator(x, amt):
    return softsign(amt * x) / softsign(amt)


def builder_atan_saturator(x, amt):
    return fast_atan(amt * x) / fast_atan(amt)


# --- Test grid ---
x = np.linspace(-1, 1, 1024)
amt = 10.0  # try changing drive amount

# --- Compute responses ---
y_ref_tanh = hyptan_saturator(x, amt)
y_fast_tanh = hyptan_saturator_fast(x, amt)

y_ref_atan = arctan_saturator(x, amt)
y_builder = builder_atan_saturator(x, amt)

y_softsign = softsign_saturator(x, amt)

y_reiss = soft_clip_reiss(x, amt)


# --- Plot ---
plt.figure(figsize=(10, 7))

plt.subplot(2, 2, 1)
plt.plot(x, y_ref_tanh, label="tanh saturator (ref)")
plt.plot(x, y_fast_tanh, "--", label="fast tanh saturator")
plt.legend()
plt.title("Tanh vs Fast Tanh")

plt.subplot(2, 2, 2)
plt.plot(x, y_ref_atan, label="atan saturator (ref)")
plt.plot(x, y_builder, "--", label="Builder atan saturator")
plt.legend()
plt.title("Atan vs Builder’s method")

plt.subplot(2, 2, 3)
plt.plot(x, y_softsign, label="softsign saturator")
plt.legend()
plt.title("Softsign")

plt.subplot(2, 2, 4)
plt.plot(x, y_reiss, label="Reiss soft clip")
plt.legend()
plt.title("Reiss exponential")

plt.tight_layout()
plt.show()
