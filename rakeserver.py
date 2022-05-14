# CITS3002 2022 Sem 1 - Project - rakeserver
# authors
#   - Daivik Anil 22987816
#   - Nathan Eden 22960674
#   - Reiden Rufin 22986337
#--------------------------------------------------------------------------

import socket, time, os, random
from subprocess import Popen, PIPE

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

port = int(input("input a port num "))
sock.bind(('', port))
print("Listening on port " + str(port))
sock.listen(5)

def send_data(sock, data):
    time.sleep(os.getpid() % 5 + 2)
    print("Sending back data to client " + data)
    sock.send(data.encode())

def send_and_receive(sock):
    data = sock.recv(1024).decode()
    print("Received from client: ", data)
    datatype = data.split(",")[0]
    data = data.split(",")[1]
    if (datatype == "quote"):
        #TODO: need to create an actual quoting calculation
        data = str(socket.gethostbyname(socket.gethostname())) + "," + str(port) + "," + str(random.randint(0,100))
        send_data(sock, data)
    if (datatype == "action"):
        data = "action!!"
        send_data(sock, data)
        #pid = os.fork()
        #if (pid == 0):
            #command = Popen(data, shell=True, stdout=PIPE)
            #output = command.communicate()[0].decode('UTF-8')
            #if (output != ''): data = output
            #send_data(data)
            #sock.close()

    else:
        #TODO: figure out how to return errors I guess?
        pass
    
keep_going = True

while keep_going:
    c, address = sock.accept()
    if c is None:
        print("Connection failed")
        keep_going = False
    print("connected from:", address)
    send_and_receive(c)

    #pid = os.fork()

    #if pid == 0:
        #getpid = os.getpid()
        #send_and_receive(c, getpid)
    #else:
        #getpid = os.getpid()
        #send_and_receive(c, getpid)
      
 
