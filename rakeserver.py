# CITS3002 2022 Sem 1 - Project - rakeserver
# authors
#   - Daivik Anil 22987816
#   - Nathan Eden 22960674
#   - Reiden Rufin 22986337
#--------------------------------------------------------------------------

import socket, os, random
from subprocess import Popen, PIPE

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

port = int(input("Input a port number to listen on: "))
sock.bind(('', port))
print("Listening on port " + str(port))
sock.listen(10)
sock.setblocking(0)

process_count = 0
action_list = []


def run_action(data):
    global process_count
    process_count += 1

    client_socket = data[0]
    data = data[1]

    # RECEIVES: "action,actioncommand"
    # SENDS: "message size,exit-code,out,err,file-output count"
    # out -> 1 if an output exists, err -> 1 if an error exists
    # SENDS: "message size,output" (if out = 1)
    # SENDS: "message size,error" (if err = 1)
    # SENDS: "message size,filepath+name,filecontent" (for every file found, so far we just send 3 dummy files)

    print("\nEXECUTING--> " + data)
    pid = os.fork()

    if pid == 0:
        command = Popen(data, shell=True, stdout=PIPE, stderr=PIPE)
        stdout, stderr = command.communicate()
        # Code that didn't get fully implemented due to time constraints.
        # file_list = ['filename0.txt,file data','filename1.txt,file data','filename2.txt,file data']

        out, err = '0', '0'
        if stdout: out = '1'
        if stderr: err = '1'

        # Sends the initial data payload.
        # "datasize,exitcode,stdout,stderr,filecount"
        # stdout/stderr = 1 when they are present
        data = ",".join([str(command.returncode), out, err, '0'])
        send_data_with_size(client_socket, data)

        # Sends the stdout and stderr if they exist.
        if stdout: send_data_with_size(client_socket, stdout.decode())
        if stderr: send_data_with_size(client_socket, stderr.decode())

        # Sends the data contained within each file. (not implemented)
        # send_batch_data_with_size(client_socket, file_list)

        # Ends the child process.
        os._exit(0)


# Sends data to the client.
def send_data(sock, data):
    if data[-1] == '\n':
        data = data[:-1]

    print("OUTGOING--> " + data)
    sock.sendall(data.encode())


# Sends a batch of data to the client with the datasize appended to the beginning of each individual message.
# data_size,data is the format. (but with multiple messages appended to each other)
def send_data_with_size(sock, data):
    if data[-1] == '\n':
        data = data[:-1]

    data_size = str(len(data))
    data = data_size + "," + data

    print("OUTGOING--> " + data)
    sock.sendall(data.encode())


def send_and_receive(sock):
    global process_count
    global action_list

    data = sock.recv(1024).decode()
    print("INCOMING<-- ", data)
    datatype = data.split(",")[0]
    data = data.split(",", 1)[1]

    # Checks the incoming message's data type.
    # quote -> quotes the server for a cost value.
    # action -> executes an action on the server.
    if datatype == "quote":
        # RECEIVES: "quote,hostname,portnum"
        # SENDS: "hostname,portnum,cost"

        # Generates a cost with a random value and appends it onto the host information.
        data = data + "," + str(random.randint(0,100))
        send_data(sock, data)
    elif datatype == "action":
        action_list.append((sock,data))

    else:
        print("ERROR--> unknown data type.")


keep_going = True
while keep_going:
    try:
        c, address = sock.accept()
        if c is None:
            print("Connection to a client failed.")
            keep_going = False
        print("CONNECTION--> ", address)
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
