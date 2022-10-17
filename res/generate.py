import numpy as np
from scipy.ndimage import gaussian_filter
    
N = 20000                               #Number of samples
samplerate = 288e3                      #ADC samplerate (currently 288k)
bitrate = 9600                          #RTT transfer rate
samPerSym = int(samplerate / bitrate)        #Samples per symbol (Needs to match liquid)
fCarrier = 25e3                         #Carrier frequency
bytes=[0xAA, 0xAA, 0xAA, 0xAA]          #Actual message bytes
magnitude = 10
dcOffset = 200
deviation = 4.8e3
sigma = 0

testData = np.array(bytes, dtype=np.uint8)
testData = np.append(testData, testData)
testData = np.append(testData, testData)
testData = np.append(testData, testData)

testBits = np.unpackbits(testData)

levelArray = np.array([])
for i in range(0, len(testBits)):
    bitsInSym = np.ones(samPerSym) * (-1) * np.power(-1, testBits[i])
    levelArray = np.append(levelArray, bitsInSym)

levelArray = levelArray * deviation
levelArray = gaussian_filter(levelArray, sigma=0.0)
freqArray = fCarrier + levelArray
smplArray = np.linspace(1,len(freqArray), len(freqArray))
wave = magnitude * np.exp(1j * 2 * np.pi * freqArray * smplArray / samplerate)

waveI = np.zeros(len(wave))
waveQ = np.zeros(len(wave))

with open("signal.txt", "w") as file:
    file.write(str(len(wave)))
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
