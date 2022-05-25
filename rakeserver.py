# CITS3002 2022 Sem 1 - Project - rakeserver
# authors
#   - Daivik Anil 22987816
#   - Nathan Eden 22960674
#   - Reiden Rufin 22986337
#--------------------------------------------------------------------------

import socket, os, random
from subprocess import Popen, PIPE

# Initialises the socket.
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Sets the port number to whatever number is input.
try:
    port = int(input("Input a port number to listen on: "))
except ValueError:
    print("A non-integer port number was entered.")
    exit(1)

# Binds the socket to the chosen port number.
try:
    sock.bind(('', port))
except OSError:
    print("The chosen port is already in use.")
    exit(1)
print("Listening on port " + str(port))

# Sets blocking on the socket to false.
sock.listen(10)
sock.setblocking(0)

# Initialises some variables.
process_count = 0
action_list = []


# Runs the action input into the function. Takes in a socket and an action command in as a parameter, both of which are
# stored in a single tuple.
def run_action(data):
    global process_count
    process_count += 1

    # Gets the required data from the input data.
    client_socket = data[0]
    data = data[1]

    # RECEIVES: "action,actioncommand"
    # SENDS: "message size,exit-code,out,err,file-output count"
    # out -> 1 if an output exists, err -> 1 if an error exists
    # SENDS: "message size,output" (if out = 1)
    # SENDS: "message size,error" (if err = 1)
    # SENDS: "message size,filepath+name,filecontent" (for every file found, this is not implemented)

    # Forks the data into a new child process.
    print("\nEXECUTING--> " + data)
    pid = os.fork()

    # Runs the command on the child process.
    if pid == 0:
        # Sends the stdout and stderr into their respective variables.
        command = Popen(data, shell=True, stdout=PIPE, stderr=PIPE)
        stdout, stderr = command.communicate()
        # Code that didn't get fully implemented due to time constraints.
        # file_list = ['filename0.txt,file data','filename1.txt,file data','filename2.txt,file data']

        # Sets the variables out/err to 1 if stdout/stderr exist.
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


# Sends data to the client with the datasize appended to the beginning of the message.
# data_size,data is the format.
def send_data_with_size(sock, data):
    if data[-1] == '\n':
        data = data[:-1]

    data_size = str(len(data))
    data = data_size + "," + data

    print("OUTGOING--> " + data)
    sock.sendall(data.encode())


# Checks any incoming data from a new client and processes it according to its data type.
def send_and_receive(sock):
    global process_count
    global action_list

    # Decodes the incoming data.
    data = sock.recv(1024).decode()
    print("INCOMING<-- ", data)

    # Splits the data into its data type and data contents.
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
    # If data type is an action, append the socket and action onto the action list.
    elif datatype == "action":
        action_list.append((sock,data))
    # If none of the data types match, an error occurred.
    else:
        print("ERROR--> unknown data type.")


# Iterates forever unless a connection fails.
keep_going = True
while keep_going:
    # Enters a try/except statement, where an error is thrown if there is no client available to be accepted.
    try:
        c, address = sock.accept()
        if c is None:
            print("Connection to a client failed.")
            keep_going = False
        print("CONNECTION--> ", address)
        # Process the input data from the client.
        send_and_receive(c)
    except BlockingIOError:
        # If processes are still being run, check if the process has already ended.
        if process_count > 0:
            # -1 as the first parameter allows us to check all the child processes. os.WNOHANG prevents blocking.
            return_value = os.waitpid(-1, os.WNOHANG);
            # If the value is not (0,0) a child process has ended.
            if return_value != (0, 0):
                process_count -= 1
                # If there are actions to be run, we run the popped action.
                if len(action_list) > 0:
                    run_action(action_list.pop())
        # If there are actions to be run, we run the popped action.
        elif len(action_list) > 0:
            run_action(action_list.pop())
