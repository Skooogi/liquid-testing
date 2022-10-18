

liquid-testing
==========
A test environment for testing liquid-dsp and GMSK signal generation. Setup..

1. Install liquid-dsp

Download repo form liquid-dsp github.
When installing with their directions, use configure command:
````
./bootstrap.sh
./configure --enable-simdoverride --enable-fftoverride --enable-strict
make
````

Next create an additional folder 'thirdparty' and place the 'liquid-dsp' folder there. 
Also remove 'libliquid.so' from 'liquid-dsp' folder.

Liquid-dsp is now installed. 

2. Usage

In the project folder ('liquid-testing'), run:
````
python3 res/generate.py
make -j 
./test_liquid.bin
python3 res/visualize.py res/processed.txt
````
To view the generated test signal:
````
python3 res/visualize.py res/signal.txt
````
Happy debugging!!