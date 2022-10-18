import matplotlib.pyplot as plt
import numpy as np
from scipy.fftpack import fft
import sys

if len(sys.argv) < 2:
    print("Please give a valid file to visualize!")
    exit() 

start_step = 0
end_step = 1000

if len(sys.argv) == 3:
    start_step = int(sys.argv[2])

if len(sys.argv) == 4:
    end_step = int(sys.argv[3]) - 1

data_x = np.array([])
data_y = np.array([])
metadata = np.array([])
steps = 0
with open(sys.argv[1], "r") as file:
    line = file.readline()
    steps = int(line[:-1])
    print("Opened file with " + str(steps) + " steps!")
    print("Processing data...")
    for i in range(steps):
        line = file.readline()
        sliced = line[:-1].split(',')
        print("STEP " + sliced[1] + " " + sliced[2] + " " + sliced[3])
        metadata = np.append(metadata, sliced)
        #print(metadata)
        for j in range(int(sliced[3])):
            line = file.readline()
            sliced = line[:-1].split(',')
            data_x = np.append(data_x, float(sliced[0]))
            data_y = np.append(data_y, float(sliced[1]))

    file.close()

print("Done!\nGenerating plots...")
offset = 0

for i in range(steps):
    N = int(metadata[3+4*i])
    samplerate = 288e3
    S = 1 / samplerate

    x = data_x[offset:offset+int(metadata[3+4*i])]
    y = data_y[offset:offset+int(metadata[3+4*i])]
    x2 = data_x[offset:offset+int(metadata[3+4*i])]
    y2 = data_y[offset:offset+int(metadata[3+4*i])]

    offset += N
    read_complex = x + 1j * y
    yf = fft(read_complex)
    freq = np.linspace(-1.0 // (2.0 * S), 1.0 // (2.0 * S), N)
    y = np.concatenate((np.abs(yf[N // 2:]), np.abs(yf[:N // 2])))

    print(str(start_step) + " " + str(end_step))
    
    if(end_step >= i and i >= start_step):
        plt.title("STEP " + str(int(metadata[1+4*i]) + 1))
        plt.plot(freq, 10*np.log(y))
        plt.grid(True)
        plt.figure()
        plt.title("STEP " + metadata[1+4*i])
        plt.axhline(0,color="grey")
        plt.axvline(0,color="grey")
        plt.plot(x2.astype(float), y2.astype(float), "o", ms=2)
        plt.grid(True)
        if(i != steps-1 and i != end_step):
            plt.figure()
plt.show()
