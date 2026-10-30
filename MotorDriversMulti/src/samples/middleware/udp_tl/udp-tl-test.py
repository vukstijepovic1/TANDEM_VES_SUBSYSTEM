import socket
import time

UDP_IP = "192.168.1.181"
UDP_PORT = 2049
REMOTE_UDP_IP = "192.168.1.100"
REMOTE_UDP_PORT = 2048

SBUS_START_BYTE = 0x0f
SBUS_END_BYTE = 0x00
SBUS_FRAME_LEN = 25
SBUS_SERVO_CH_NUM = 16
SBUS_TOGGLE_CH_NUM = 2

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.settimeout(1)
sock.bind((UDP_IP, UDP_PORT))

sbusChannels = [0] * SBUS_SERVO_CH_NUM

chData = 0x7FE
digi = 0

while True:
    sbusPacket = bytearray(SBUS_FRAME_LEN)

    for i in range(SBUS_SERVO_CH_NUM):
        sbusChannels[i] = chData

    chData = (chData + 1) & 0x7FF

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

    sock.sendto(sbusPacket, (REMOTE_UDP_IP, REMOTE_UDP_PORT))
    try:
        arr = sock.recvfrom(4096)

        for i in range(0, len(arr[0])):
            if arr[0][i] != sbusPacket[i]:
                print("Packet error!")
                break

        time.sleep(.02)
    except socket.timeout:
        print("Receive timeout!")
        pass