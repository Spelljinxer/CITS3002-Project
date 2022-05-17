# CITS3002 2022 Sem 1 - Project - rakeserver
# authors
#   - Daivik Anil 22987816
#   - Nathan Eden 22960674
#   - Reiden Rufin 22986337
#--------------------------------------------------------------------------

import socket, time, os, random
from subprocess import Popen, PIPE
from collections import Counter

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

port = int(input("input a port num "))
sock.bind(('', port))
print("Listening on port " + str(port))
sock.listen(10)

# Sends data to the client.
def send_data(sock, data):
    print("OUTGOING-->" + data)
    sock.sendall(data.encode())

# Sends data to the client with the datasize appended to the beginning.
# data_size,data is the format.
def send_data_with_size(sock, data):
    data_size = str(len(data))
    data = data_size + "," + data

    print("OUTGOING-->" + data)
    sock.sendall(data.encode())

def send_and_receive(sock):
    global servercost

    data = sock.recv(1024).decode()
    print("INCOMING<--", data)
    datatype = data.split(",")[0]
    data = data.split(",", 1)[1]

    # Checks the incoming message's data type.
    # quote -> quotes the server for a cost value.
    # action -> executes an action on the server.
    if (datatype == "quote"):
        servercost -= 1

        data = data.split(",",2)
        host = ",".join(data[0:2])

        # Generates a cost with a random value and appends it onto the host information.
        data = host + "," + str(random.randint(0,100))
        send_data(sock, data)
    if (datatype == "action"):
        #TODO: receive files, etc

        servercost -= 1
        pid = os.fork()
        if (pid == 0):
            #time.sleep(os.getpid() % 5 + 2)
            command = Popen(data, shell=True, stdout=PIPE, stderr=PIPE)
            stdout, stderr = command.communicate()
            fcount = 3

            out, err = '0','0'
            if stdout: out = '1'
            if stderr: err = '1'

            # Sends the initial data payload.
            # "datasize,exitcode,stdout,stderr,filecount"
            # stdout/stderr = 1 when they are present
            data = ",".join([str(command.returncode),out,err,str(fcount)])
            send_data_with_size(sock, data)

            # Sends the stdout and stderr if they exist.
            if stdout: send_data_with_size(sock, stdout.decode())
            if stderr: send_data_with_size(sock, stderr.decode())

            # Sends the data contained within each file.
            for files in range(0, fcount):
                send_data_with_size(sock, "filename" + str(files) + ".txt,no files yet!!!!")

            # Ends the child process.
            os._exit(0)

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

      
 
