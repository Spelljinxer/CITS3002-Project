# CITS3002 2022 Sem 1 - Project - rake client python
# authors
#   - Daivik Anil 22987816
#   - Nathan Eden 22960674
#   - Reiden Rufin 22986337
import socket
import select
from subprocess import Popen, PIPE
#--------------------------------------------------------------------------------------------------------------
port = 0
hosts = []
actionsets = [] 

with open('Rakefile', 'r') as f:
    setnum = -1

    for line in f:
        line = line.split("#", 1)[0].replace('\n','')
        if (line != "" and len(line.replace(chr(9),"")) != 0):
            # First check if the line is one-tabbed.
            if (line[0] == chr(9)):
                # Then check if line is two-tabbed.
                if (line[1] == chr(9)):
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
                if ("HOSTS" in line):
                    hosts += line.split("=", 1)[1].split(" ")[1:]
                # Adds the default port into the port variable.
                elif ("PORT" in line):
                    port = line.split("=")[1].replace(" ","")
                # Adds an empty array into the actionset array and then increments the current set number.
                elif (":" in line):
                    setnum += 1
                    actionnum = -1
                    actionsets.append([])

# print(port)
# print(hosts)
# print(actionsets)

remote = False
for actionset in actionsets:
    for action in actionset:
        curraction = action[0]
        
        if (action[0][:7] == "remote-"):
            remote = True
            curraction = curraction[7:]
            print("Remotely executing " + action[0][7:])
        else:
            print("Executing " + action[0])

        command = Popen(curraction, shell=True, stdout=PIPE)
        output = command.communicate()[0].decode('UTF-8')
        if (output != ''): print(output)

##-----------------------------------------------------------------------------------------------------------------

# iterate through every host + port, send message, wait for reply via select, do shit.

def quote_servers():
    min_cost = float('inf')
    min_sock = None
    connections = []

    for host in hosts:
        portnum = int(port)

        if (host.count(':') > 0):
            portnum = int(host.split(":")[1])
            host = host.split(":")[0]

        sock = socket.socket()
        sock.connect((host, portnum))

        message = "quote,None"
        sock.send(message.encode())
        connections.append(sock)

    while connections:
        ready, empty, error = select.select(connections, [], connections)

        for sock in ready:
            data = sock.recv(1024)
            if data:
                data = data.decode()

                if (int(data) < min_cost):
                    min_cost = int(data)
                    if min_sock: min_sock.close()
                    min_sock = sock
                else:
                    sock.close()

                connections.remove(sock)

    print(min_sock)
    return min_sock

quote_servers()

def send_message(socket, message):
    print("Sending message from client...")
    socket.send(message.encode())
    data = socket.recv(1024)
    data = data.decode()
    print("Got message back from server: " + data)
   
