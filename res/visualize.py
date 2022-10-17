import matplotlib.pyplot as plt
import numpy as np
from scipy.fftpack import fft
import sys

if len(sys.argv) < 2:
    print("Please give a valid file to visualize!")
    exit() 

start_step = 0
end_step = 0

if len(sys.argv) == 3:
    start_step = sys.argc[2]

if len(sys.argv) == 4:
    end_step = sys.argc[3]

waveI = np.array([])
waveQ = np.array([])
signal_length = 0;
with open(sys.argv[1], "r") as file:
    line = file.readline()
    sliced = line[:-1].split(",")
    signal_length = int(sliced[1])
    line = file.readline()
    i = 0
    print("Opened " + sys.argv[1] + "! [ " + sliced[1] + " ]")
    print("Processing data...")
    while i < signal_length:
        sliced = line[:-1].split(",")
        waveI = np.append(waveI, float(sliced[0]))
        waveQ = np.append(waveQ, float(sliced[1]))
        line = file.readline()
        if not line:
            break
        i += 1
    file.close()

print("Done!\nGenerating plots...")

N = i+1
samplerate = 288e3
S = 1 / samplerate

read_complex = waveI + 1j * waveQ
yf = fft(read_complex)
freq = np.linspace(-1.0 // (2.0 * S), 1.0 // (2.0 * S), N)
y = np.concatenate((np.abs(yf[N // 2:]), np.abs(yf[:N // 2])))

plt.plot(freq, 10*np.log(y))
plt.grid(True)
plt.figure()
plt.axhline(0,color="grey")
plt.axvline(0,color="grey")
plt.plot(waveI.astype(float), waveQ.astype(float), "o", ms=2)
plt.grid(True)
plt.show()
