#!/usr/bin/env python
import socket
import time

TCP_IP = '192.168.7.2'
TCP_PORT = 1234
BUFFER_SIZE = 1024
MESSAGE = "Hello, World!"


count = 0
while (count < 1000):
    count = count + 1
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((TCP_IP, TCP_PORT))
    s.send(MESSAGE)
    data = s.recv(BUFFER_SIZE)
    s.close()
    print "received data:", data
    time.sleep(1) # delays for 5 seconds


print "Good bye!"
