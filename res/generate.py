import numpy as np
import matplotlib.pyplot as plt
from scipy.ndimage import gaussian_filter
    
N = 20000                               #Number of samples
samplerate = 288e3                      #ADC samplerate (currently 288k)
bitrate = 9600                          #RTT transfer rate
samPerSym = int(samplerate / bitrate)        #Samples per symbol (Needs to match liquid)
fCarrier = 25e3                         #Carrier frequency
#bytes=[0xFF, 0x12, 0xA3, 0x4F]          #Actual message bytes
magnitude = 10
dcOffset = 200
deviation = 4.8e3
sigma = 0
message = np.array([0x47,0x4d,0x53,0x4b,0x44,0x4d,0x4f,0x44]) #GMSKDMOD

data = np.array(message, dtype=np.uint8)
data = np.append(data, data)
data = np.append(data, data)
data = np.append(data, data)

testBits = np.unpackbits(data)
nrzi = np.array([])
state = testBits[0]
for bit in testBits:
    state ^= bit ^ 1
    nrzi = np.append(nrzi, state)
testBits = nrzi
levelArray = np.array([])
for bit in testBits:
    bitsInSym = np.ones(samPerSym) * (-1) * np.power(-1, bit)
    levelArray = np.append(levelArray, bitsInSym)

levelArray = levelArray * deviation
freqArray = fCarrier + levelArray
smplArray = np.linspace(1, len(freqArray), len(freqArray))  # sampleArray / samplerate = time array
wave = magnitude * np.exp(1j * 2 * np.pi * freqArray * smplArray / samplerate)
waveI = np.zeros(len(wave))
waveQ = np.zeros(len(wave))

with open("signal.txt", "w") as file:
    file.write(str(1));
    file.write('\n')
    file.write("STEP,0,8,"+str(len(wave)))
    file.write('\n')
    for i in range(0, len(wave)):
        waveI[i] = wave[i].real + dcOffset
        waveQ[i] = wave[i].imag + dcOffset
        file.write(str(waveI[i]))
        file.write(",")
        file.write(str(waveQ[i]))
        file.write('\n')
    file.close()

print("Generated signal to signal.txt!")
