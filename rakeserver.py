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

port = int(input("Input a port number to listen on. "))
sock.bind(('', port))
print("Listening on port " + str(port))
sock.listen(10)
sock.setblocking(0)

process_count = 0
action_list = []

def run_action(data):
    global process_count
    process_count += 1

    sock = data[0]
    data = data[1]

    # RECEIVES: "action,actioncommand"
    # SENDS: "message size,exit-code,out,err,file-output count"
    # out -> 1 if an output exists, err -> 1 if an error exists
    # SENDS: "message size,output" (if out = 1)
    # SENDS: "message size,error" (if err = 1)
    # SENDS: "message size,filepath+name,filecontent" (for every file found, so far we just send 3 dummy files)

    print("EXECUTED-->" + data)
    pid = os.fork()

    if pid == 0:
        command = Popen(data, shell=True, stdout=PIPE, stderr=PIPE)
        stdout, stderr = command.communicate()
        fcount = 3

        out, err = '0', '0'
        if stdout: out = '1'
        if stderr: err = '1'

        # Sends the initial data payload.
        # "datasize,exitcode,stdout,stderr,filecount"
        # stdout/stderr = 1 when they are present
        data = ",".join([str(command.returncode), out, err, str(fcount)])
        send_data_with_size(sock, data)

        # Sends the stdout and stderr if they exist.
        if stdout: send_data_with_size(sock, stdout.decode())
        if stderr: send_data_with_size(sock, stderr.decode())

        # Sends the data contained within each file.
        for files in range(0, fcount):
            send_data_with_size(sock, "filename" + str(files) + ".txt,no files yet!!!!")

        # Ends the child process.
        os._exit(0)


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
    global process_count
    global action_list

    data = sock.recv(1024).decode()
    print("INCOMING<--", data)
    datatype = data.split(",")[0]
    data = data.split(",", 1)[1]

    # Checks the incoming message's data type.
    # quote -> quotes the server for a cost value.
    # action -> executes an action on the server.
    if (datatype == "quote"):
        # RECEIVES: "quote,hostname,portnum"
        # SENDS: "hostname,portnum,cost"

        # Generates a cost with a random value and appends it onto the host information.
        data = data + "," + str(random.randint(0,100))
        send_data(sock, data)
    if (datatype == "action"):
        action_list.append((sock,data))

    else:
        #TODO: figure out how to return errors I guess?
        pass


keep_going = True
while keep_going:
    try:
        c, address = sock.accept()
        if c is None:
            print("Connection failed")
            keep_going = False
        print("connected from:", address)
        send_and_receive(c)
    except BlockingIOError:
        if process_count > 0:
            return_value = os.waitpid(-1, os.WNOHANG);
            if return_value != (0, 0):
                process_count -= 1
                if len(action_list) > 0:
                    run_action(action_list.pop())
        elif len(action_list) > 0:
            run_action(action_list.pop())
