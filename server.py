import socket
import time
from fastapi import FastAPI


def createServer():                         #Create a server to recieve
    global sock                             #data from the Arduino
    sock = socket.socket()
    host = socket.gethostname()
    print("Initializing Server")
    print("Host:", host)
    port = 8000
    print("Port:", port)
    sock.bind((host, port))
   # print(sock.bind((host, port)))
    sock.listen(5)
    while 1:
        c, addr = sock.accept()
        c.send("Connected!")
        print("Connected to "+addr[0]+":"+str(addr[1]))
    print(c.recv(1024))


createServer()
