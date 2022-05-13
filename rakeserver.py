# CITS3002 2022 Sem 1 - Project - rakeserver
# authors
#   - Daivik Anil 22987816
#   - Nathan Eden 22960674
#   - Reiden Rufin 22986337
#--------------------------------------------------------------------------

import socket
import time
import os

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

port = 6238
sock.bind(('', port))
print("Listening on port " + str(port))
sock.listen(5)

def send_and_receive(sock, pid):
    data = sock.recv(1024).decode()
    print("Received from client: ", data)
    time.sleep(os.getpid() % 5 + 2)
    print("Sending back data to client from pid: " + str(pid))
    sock.send(data.encode())
    
keep_going = True

while keep_going:
    c, address = sock.accept()
    if c is None:
        print("Connection failed")
        keep_going = False
    print("connected from:", address)
    pid = os.fork()

    if pid == 0:
        getpid = os.getpid()
        send_and_receive(c, getpid)
    else:
        getpid = os.getpid()
        send_and_receive(c, getpid)
      
 
