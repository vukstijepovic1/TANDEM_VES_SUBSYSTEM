import socket
import time

UDP_IP = "192.168.1.181"
UDP_PORT = 2049
REMOTE_UDP_IP = "192.168.1.100"
REMOTE_UDP_PORT = 2048
MESSAGE = b"Hello!"

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

while True:
    sock.sendto(MESSAGE, (REMOTE_UDP_IP, REMOTE_UDP_PORT))
    rx_data = sock.recvfrom(4096)

    for i in range(0, len(MESSAGE)):
        if (rx_data[0][i] != MESSAGE[i]):
            print("Packet error!")
            break

    time.sleep(.02)