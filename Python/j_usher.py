#
#
# 'j_usher.py' pushes (or ushers) measured signal data over RTT to AIS-SDR prototype.
# for debugging digital signal processing chain
#
#

import pylink           # for JLink and RTT session
import numpy as np
import time

import lil_endian       # simple func. flips and parses the read data bytes


serial_num = 801003397  # either JLink or device specific (not sure, but works)
tgt_device = 'STM32H743ZI'
bytes_per_int = 2       # Bytes per in one integer of data. int_16 = 2. uint_32 = 4.

# Initiate JLink session and print session info.
jlink = pylink.JLink()
jlink.open(serial_no=serial_num)
print(jlink.product_name)
print(f"S/N {jlink.serial_number}")
jlink.connect(tgt_device)
con = jlink.target_connected()
print(f"Target connected: {con}")
if not con:                         # if target device is not found, stop execution
    print("Target not found!")
    exit()

# Ask user to start RTT manually, since device has to be running first
print(f"\nMAKE SURE DEVICE HAS INITIATED RTT (i.e. is running)")
user = input("Start RTT and sending data? y or n: ")
if user != 'y':     # User stops execution if they don't initiate RTT
    print("RTT not initiated. Stopping execution.")
    exit()
jlink.rtt_start()
print(f"RTT start")
print(f"RTT satus: {jlink.rtt_get_status()}")       # intended typo

bytes_per_int = 2                                   # as per int16_t now, should be float though
i_data = lil_endian.txt_read("hI.txt")              # Saved signal from .txt file
q_data = lil_endian.txt_read("hQ.txt")              # Saved signal from .txt file


# Current implementation is a loop that sends test signal to RTT,
# sleeps for while (as DSP happens on µC), and then tries to read
# what the µC is sending back.
# Should maybe be done with threads instead.
while True:
    jlink.rtt_write(1, i_data)  # write data (as bytes) to RTT down-buffer '1'
    jlink.rtt_write(2, q_data)  # write data (as bytes) to RTT down-buffer '2'
                                ## DOES DATA NEED TO BE IN BYTES ALREADY??
    time.sleep(0.5)             # sleep for half a second

    rtt_bytes = jlink.rtt_read(1, len(i_data) * bytes_per_int)      # Read data in bytes from RTT
    data = lil_endian.byte_parser(rtt_bytes, bytes_per_int, False)  # Bytes to integers

    # Plotting
    samples = np.arange(1, len(data) + 1)  # 'samples' gives indexes to read data
    plt.figure()
    plt.plot(samples, data, '.k')
    plt.xlabel("sample")
    plt.ylabel("RTT Data [unit]")
    plt.title(f"Num. of integers read from RTT: {len(data)}")
    plt.show()
