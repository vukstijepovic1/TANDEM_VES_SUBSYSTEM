import serial
import numpy as np
import time

ser = serial.Serial('/dev/ttyUSB0')
ser.baudrate = 200000
ser.Timeout = 1

tx = np.zeros((1, 133), dtype = np.uint8)

while (1):
    file = open("./src/samples/drivers/motor/motor-data.txt", 'r')
    for line in file:
        time.sleep(0.02)

        for i in range(0, 133):
            tx[0][i] = np.uint8(int(line[i * 3] + line[i * 3 + 1], 16))

        ser.write(tx)

    file.close()