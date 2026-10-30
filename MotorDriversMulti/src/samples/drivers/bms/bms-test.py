import serial
import numpy as np
import time

ser = serial.Serial('/dev/ttyUSB0')
ser.baudrate = 9600
ser.Timeout = 1

resp = np.array([int("0xD2", 16), int("0x03", 16), int("0x7C", 16), # header
                 int("0x10", 16), int("0x2C", 16),  # bat 1 volt
                 int("0x10", 16), int("0x30", 16),  # bat 2 volt
                 int("0x10", 16), int("0x2B", 16),  # bat 3 volt
                 int("0x10", 16), int("0x2D", 16),  # bat 4 volt
                 int("0x10", 16), int("0x2E", 16),  # bat 5 volt
                 int("0x10", 16), int("0x30", 16),  # bat 6 volt
                 int("0x10", 16), int("0x2E", 16),  # bat 7 volt
                 int("0x10", 16), int("0x2E", 16),  # bat 8 volt
                 int("0x10", 16), int("0x2A", 16),  # bat 9 volt
                 int("0x10", 16), int("0x2C", 16),  # bat 10 volt
                 int("0x10", 16), int("0x2C", 16),  # bat 11 volt
                 int("0x10", 16), int("0x28", 16),  # bat 12 volt
                 int("0x10", 16), int("0x2C", 16),  # bat 13 volt
                 int("0x00", 16), int("0x00", 16),  # bat 14 volt
                 int("0x00", 16), int("0x00", 16),  # bat 15 volt
                 int("0x00", 16), int("0x00", 16),  # bat 16 volt
                 int("0x00", 16), int("0x00", 16),  # bat 17 volt
                 int("0x00", 16), int("0x00", 16),  # bat 18 volt
                 int("0x00", 16), int("0x00", 16),  # bat 19 volt
                 int("0x00", 16), int("0x00", 16),  # bat 20 volt
                 int("0x00", 16), int("0x00", 16),  # bat 21 volt
                 int("0x00", 16), int("0x00", 16),  # bat 22 volt
                 int("0x00", 16), int("0x00", 16),  # bat 23 volt
                 int("0x00", 16), int("0x00", 16),  # bat 24 volt
                 int("0x00", 16), int("0x00", 16),  # bat 25 volt
                 int("0x00", 16), int("0x00", 16),  # bat 26 volt
                 int("0x00", 16), int("0x00", 16),  # bat 27 volt
                 int("0x00", 16), int("0x00", 16),  # bat 28 volt
                 int("0x00", 16), int("0x00", 16),  # bat 29 volt
                 int("0x00", 16), int("0x00", 16),  # bat 30 volt
                 int("0x00", 16), int("0x00", 16),  # bat 31 volt
                 int("0x00", 16), int("0x00", 16),  # bat 32 volt
                 int("0x00", 16), int("0x3E", 16),  # T1
                 int("0x00", 16), int("0x00", 16),  #
                 int("0x00", 16), int("0x00", 16),  #
                 int("0x00", 16), int("0x00", 16),  #
                 int("0x00", 16), int("0x00", 16),  #
                 int("0x00", 16), int("0x00", 16),  #
                 int("0x00", 16), int("0x00", 16),  #
                 int("0x00", 16), int("0x00", 16),  #
                 int("0x02", 16), int("0x1A", 16),  # Sum Volt
                 int("0x75", 16), int("0x30", 16),  # Current
                 int("0x02", 16), int("0x30", 16),  # SOC
                 int("0x10", 16), int("0x30", 16),  # Maximum volt
                 int("0x10", 16), int("0x29", 16),  # Minimum volt
                 int("0x00", 16), int("0x3E", 16),  # Temp of max volt
                 int("0x00", 16), int("0x3E", 16),  # Temp of min volt
                 int("0x00", 16), int("0x00", 16),  #
                 int("0x00", 16), int("0xA8", 16),  #
                 int("0x00", 16), int("0x0D", 16),  # Battery String
                 int("0x00", 16), int("0x01", 16),  #
                 int("0x00", 16), int("0x00", 16),  #
                 int("0x00", 16), int("0x00", 16),  #
                 int("0x00", 16), int("0x01", 16),  #
                 int("0x00", 16), int("0x01", 16),  #
                 int("0x10", 16), int("0x2A", 16),  # Average Volt
                 int("0x00", 16), int("0x07", 16),  # Diff Volt
                 int("0x00", 16), int("0x00", 16),  #
                 int("0x00", 16), int("0x00", 16),  #
                 int("0x00", 16), int("0x00", 16),  #
                 int("0x00", 16), int("0x00", 16),  #
                 int("0x00", 16), int("0x00", 16),  #
                 int("0x5D", 16), int("0xDE", 16),  # CRC
                 ], dtype = np.uint8)

time_sent = time.time()

while (1):
    if time.time() - time_sent >= 0.5:
        time_sent = time.time()

        try:
            ser.write(resp)

        except serial.SerialTimeoutException:
            pass
