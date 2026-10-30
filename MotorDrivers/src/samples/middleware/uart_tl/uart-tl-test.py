import serial
import time

SBUS_START_BYTE = 0x0f
SBUS_END_BYTE = 0x00
SBUS_FRAME_LEN = 25
SBUS_SERVO_CH_NUM = 16
SBUS_TOGGLE_CH_NUM = 2

ser = serial.Serial('/dev/ttyUSB0', 100000, stopbits = 2, \
                    parity=serial.PARITY_EVEN)

sbusChannels = [0] * SBUS_SERVO_CH_NUM
for i in range(SBUS_SERVO_CH_NUM):
    sbusChannels[i] = 0x0F

digi = 0

while True:
    sbusPacket = bytearray(SBUS_FRAME_LEN)

    sbusPacket[0] = SBUS_START_BYTE

    i = 1
    off = 0
    chVal = 0
    for ch in sbusChannels:
        chVal |= ch << off
        off += 11

        while off >= 8:
            sbusPacket[i] = chVal & 0xFF
            chVal >>= 8
            off -= 8
            i += 1

    sbusPacket[23] = (digi << 0) | (digi << 1) | (digi << 2) | (digi << 3)
    sbusPacket[24] = SBUS_END_BYTE

    ser.write(sbusPacket)
    arr = ser.readline()

    for i in range(0, len(arr) - 2):
        if arr[i] != sbusPacket[i]:
            print("Packet error!")
            break

    time.sleep(.02)