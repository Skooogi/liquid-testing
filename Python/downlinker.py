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
i_data = lil_endian.txt_reader("hI.txt")              # Saved signal from .txt file
q_data = lil_endian.txt_reader("hQ.txt")              # Saved signal from .txt file

print(f"I data read from txt: {i_data}")

print(f"Q data read from txt: {q_data}")
# Current implementation is a loop that sends test signal to RTT,
# sleeps for while (as DSP happens on µC), and then tries to read
# what the µC is sending back.
# Should maybe be done with threads instead.
while True:
    i_bytes = lil_endian.bytes_from_data(i_data, bytes_per_int, False)
    q_bytes = lil_endian.bytes_from_data(q_data, bytes_per_int, False)
    print(f"I data as bytes: {i_bytes[0:19]}")

    print(f"Q data as bytes: {q_bytes[0:19]}")

    jlink.rtt_write(1, i_bytes[0:19])  # write data (as bytes) to RTT down-buffer '1'
    jlink.rtt_write(2, q_bytes[0:19])  # write data (as bytes) to RTT down-buffer '2'

    print(f"RTT buff I descrips: {jlink.rtt_get_buf_descriptor(1, False)}")
    print(f"RTT buff Q descrips: {jlink.rtt_get_buf_descriptor(2, False)}")
    time.sleep(10.0)
    print("Slept for 10 seconds. Inputting more.")

