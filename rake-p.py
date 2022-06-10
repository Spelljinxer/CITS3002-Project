# CITS3002 2022 Sem 1 - Project - rake client python
# authors
#   - Hidden for Privacy.
#   - Hidden for Privacy.
#   - Hidden for Privacy.
import socket
import select
#--------------------------------------------------------------------------------------------------------------

# Initialises some variables.
port = 0
hosts = []
actionsets = []


# Quotes all the stored hosts to find the minimum costing server.
def quote_servers():
    min_cost = float('inf')
    connections = []

    # Iterates through every host, sending a quote message to each.
    for host in hosts:
        portnum = int(port)

        # If the host has no port added onto it, the default port is set to it.
        if host.count(':') > 0:
            portnum = int(host.split(":")[1])
            host = host.split(":")[0]

        # Connects to the host and sends a message.
        sock = socket.socket()
        try:
            sock.connect((host, portnum))

            message = "quote," + host + "," + str(portnum)
            sock.sendall(message.encode())
            connections.append(sock)
        except ConnectionRefusedError:
            # If a connection fails, exit the program with an error.
            print("Connection to a host failed.")
            exit(1)

    # While connections remain, keep iterating to find costs from hosts.
    while connections:
        ready, empty, error = select.select(connections, [], connections)

        # Iterates through every ready socket.
        for sock in ready:
            data = sock.recv(1024)
            if data:
                data = data.decode().split(",")

                # Gets info from the received message. The message is in the form of "hostname,portnum,cost"
                sockinfo = (data[0], int(data[1]))
                cost = int(data[2])

                # If the found cost is under the minimum cost, store the new info.
                if cost < min_cost:
                    min_cost = cost
                    min_sock = sockinfo

                sock.close()
                connections.remove(sock)

    return min_sock

# Processes the rakefile, storing it into lists.
def parse_rakefile():
    global port
    global hosts
    global actionsets

    with open('Rakefile', 'r') as f:
        setnum = -1

        for line in f:
            line = line.split("#", 1)[0].replace('\n','')
            if line != "" and len(line.replace(chr(9),"")) != 0:
                # First check if the line is one-tabbed.
                if line[0] == chr(9):
                    # Then check if line is two-tabbed.
                    if line[1] == chr(9):
                        # The 'requires' word is ignored, under the assumption of valid syntax.
                        actionsets[setnum][actionnum] += line.lstrip().split(" ")[1:]
                    # If it isn't, we assume it is one-tabbed.
                    else:
                        # The current action index is incremented and an action is added.
                        actionnum += 1
                        actionsets[setnum].append([line.lstrip()])

                # Then, if none, it must either be a PORT, HOSTS or actionset line.
                else:
                    # Strips any whitespace from the line.
                    # TODO: this may not even be needed
                    line = line.strip()

                    # Adds the hosts into the hosts variable.
                    if "HOSTS" in line:
                        hosts += line.split("=", 1)[1].split(" ")[1:]
                    # Adds the default port into the port variable.
                    elif "PORT" in line:
                        port = line.split("=")[1].replace(" ","")
                    # Adds an empty array into the actionset array and then increments the current set number.
                    elif ":" in line:
                        setnum += 1
                        actionnum = -1
                        actionsets.append([])

# Processes the actions stored from the Rakefile.
def process_actions():
    global port
    global hosts
    global actionsets

    # Iterates through every actionset in the parsed Rakefile data.
    for s_index, actionset in enumerate(actionsets):
        error_found = False
        connections = []

        # Iterates through every action in the current actionset, connecting to a host server to execute it.
        for a_index, action in enumerate(actionset):
            # Selects the action command in the array for the action. (subsequent elemements are its requirements)
            curraction = action[0]

            # Executes non-remote actions on the localhost. Remote actions are executed on the lowest costing server,
            # The cost is determined via quote_servers().
            if (action[0][:7] == "remote-"):
                curraction = curraction[7:]
                print("R-OUTGOING--> " + curraction)
                sockinfo = quote_servers()
                sock = socket.socket()
                # If a connection fails, the client exits with an error code of 1.
                try:
                    sock.connect(sockinfo)
                except ConnectionRefusedError:
                    print("Connection to a host failed.")
                    quit(1)
            else:
                print("OUTGOING--> " + curraction)
                sock = socket.socket()
                try:
                    sock.connect(('localhost', int(port)))
                # If a connection fails, the client exits with an error code of 1.
                except ConnectionRefusedError:
                    print("Connection to a host failed.")
                    quit(1)

            # Data is sent to the connected socket and the socket is stored in a list of open connections.
            message = "action," + curraction
            sock.sendall(message.encode())
            connections.append(sock)

        action_count = 0
        # Iterates through every socket connected to a server, checking if any data is ready to be received.
        while connections:
            ready, empty, error = select.select(connections, [], connections)
            for sock in ready:
                action_count += 1
                print("\nNEW-ACTION--> " + str(action_count))
                # For each socket, the initial data payload is first read.
                # "datasize,exitcode,stdout,stderr,filecount"
                # stdout/stderr = 1 when they are present
                data_left = float('inf')
                f_data = ""
                extra_data = ""

                # While data is left in the current message, keep receiving more data.
                while data_left > 0:
                    data = sock.recv(1024)

                    # If data exists, decode the data.
                    if data:
                        data = data.decode()

                        # If data left is infinite, this is the first received data in the message.
                        # Thus, the first comma-separated value is the size of the first part of the message.
                        if data_left == float('inf'):
                            # Sets the data to whatever the remainder of the data after the data size value is.
                            data = data.split(",")
                            data_left = int(data[0])
                            data = ",".join(data[1:])

                        # If this is true, some additional data was sent through.
                        if len(data) > data_left:
                            # Sets the extra data to whatever is after the original message.
                            extra_data = data[data_left:]
                            data = data[:data_left]
                            data_left = 0
                        else:
                            data_left -= len(data)

                        # Increments found data onto the total f_data (final data).
                        f_data += data

                # Splits the initial message into four values.
                # data_exitcode is the commands exit code.
                # data_stdout is 1 when stdout exists and 0 otherwise.
                # data_stderr is 1 when stderr exists and 0 otherwise.
                # data_fcount is the number of files that were returned. (file sending not fully implemented)
                f_data = f_data.split(",")
                data_exitcode = int(f_data[0])
                data_stdout = int(f_data[1])
                data_stderr = int(f_data[2])
                data_fcount = int(f_data[3])

                if data_exitcode != 0:
                    error_found = True

                if data_stdout == 1:
                    # Reads additional messages from the same socket.
                    output, extra_data = read_data(sock, extra_data, False)
                    print("OUTPUT--> " + output)
                else:
                    print("OUTPUT--> " + "None")

                if data_stderr == 1:
                    # Reads additional messages from the same socket.
                    output, extra_data = read_data(sock, extra_data, False)
                    print("ERROR--> " + output)

                # File reading was not fully implemented.
                for files in range(0, data_fcount):
                    extra_data = read_data(sock, extra_data)

                # Closes the current socket and removes it from the open connections list.
                sock.close()
                connections.remove(sock)

        # Prematurely terminate the program if any action fails.
        if error_found:
            print("\nTERMINATED--> actionset " + str(s_index + 1) + " returned errors")
            break

# Reads more data from a socket, whilst also taking in any prior extra data from previous reads.
def read_data(sock, extra_data, is_file=True):
    data_left = float('inf')
    f_data = ""

    # If extra data is input, process it.
    if extra_data:
        # Find the data left from the first comma-separated value.
        extra_data = extra_data.split(",")
        data_left = int(extra_data[0])
        f_data = ",".join(extra_data[1:])

        # If the received data once again exceeds the data required, the new excess is stored in extra data again.
        if len(f_data) > data_left:
            extra_data = f_data[data_left:]
            f_data = f_data[:data_left]
            
            data_left = 0
        # If not, the extra data is set to an empty string - indicating that more data can be received from the socket.
        else:
            extra_data = ""
            data_left -= len(f_data)

    # If there is still data to retrieve from the socket, receive more data.
    while data_left > 0:
        data = sock.recv(1024)
        if data:
            data = data.decode()
            # The same processes occur as the ones that occurred within the earlier read data calls.
            if data_left == float('inf'):

                data = data.split(",")
                data_left = int(data[0])
                data = ",".join(data[1:])

            if len(data) > data_left:
                extra_data = data[data_left:]
                data = data[:data_left]
                data_left = 0
            else:
                data_left -= len(data)
            
            f_data += data

    # If the data is not a file, return it. If not, write the data to a file.
    # (file writing is implemented but there is no implementation of server's sending files to clients)
    if not is_file: return f_data, extra_data
    else:
        f_data = f_data.split(",")
        file = open(f_data[0],'w')
        file.write(f_data[1])
        file.close()
        print("FILE--> " + f_data[0])
        return extra_data

# Runs the methods required to process the Rakefile and its stored actions.
parse_rakefile()
process_actions()
