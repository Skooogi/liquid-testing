# ***
# Function for reading the contents of .log logger file created by RTT viewer
# from buffer print.

import numpy as np

# Reading the contents of .log logger file created by RTT viewer
# from terminal.


def read_data(filename):
    # open file and read each line into array
    file1 = open(filename, "r")
    lines_full = file1.readlines()
    # drop the header lines
    lines = lines_full[5:]

    # First line has the number of rtt_bytes lines. Useful failsafe if
    # terminal logging was stopped later than intended.
    data_len_line = lines[0].partition(' ')
    n_data = int(data_len_line[2].rstrip('\n'))

    # Take only measurement rtt_bytes
    data_lines = lines[1:n_data+1]
    # Prepare a zeros array for rtt_bytes
    data = np.zeros(n_data)

    # Read actual rtt_bytes from the data_lines into 'rtt_bytes' array
    for i in range(0, n_data):
        data_separated = data_lines[i].partition(' ')
        data[i] = float(data_separated[2].rstrip('\n'))

    return data
