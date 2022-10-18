

liquid-testing
==========
A test environment for testing liquid-dsp and GMSK signal generation. Setup..

1. Clone this repo recursively
````
git clone --recursive https://github.com/Skooogi/liquid-testing.git
````


2. Building liquid-dsp for your system

Go to folder thirdparty/liquid-dsp/
Install with their directions, using our configure command:
````
sudo apt-get install automake autoconf
./bootstrap.sh
./configure --enable-simdoverride --enable-fftoverride --enable-strict
make
````
Liquid-dsp is now built. 

3. Running the test scripts.

In the project folder ('liquid-testing'), run:
````
make run
````
And there you go!

Alternatively, you can run each script manually (not rly recommended though):
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
