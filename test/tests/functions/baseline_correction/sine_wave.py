#!/usr/bin/env python3
import numpy as np

dt = 0.01
time = np.arange(-5.0, 5.0 + dt, dt)
accel = 80 * np.square(np.pi) * np.sin(4.0 * np.pi * time)

np.savetxt("sine_wave.csv", time, fmt="%0.2f", delimiter=",", newline=", ")
with open("sine_wave.csv", "a") as file:
    file.write("\n")
    np.savetxt(file, accel, fmt='%0.18e', delimiter=",", newline=", ")
