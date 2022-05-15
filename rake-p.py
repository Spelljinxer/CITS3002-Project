# CITS3002 2022 Sem 1 - Project - rake client python
# authors
#   - Daivik Anil 22987816
#   - Nathan Eden 22960674
#   - Reiden Rufin 22986337
import socket
import select
from asyncio import subprocess
from subprocess import Popen, PIPE
import subprocess
#--------------------------------------------------------------------------------------------------------------
port = 0
hosts = []
actionsets = []

# i put them in functions just to make it easier to understand, we can reorganise it later

# quotes servers for costs and shit
def quote_servers():
    min_cost = float('inf')
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
                data = data.decode().split(",")
                #print("data is: ", data)
                data[1] = int(data[1])
                cost = int(data[2])

                if (cost < min_cost):
                    min_cost = cost
                    min_sock = tuple(data[0:2])

                sock.close()
                connections.remove(sock)

    return min_sock

def parse_rakefile():
    #TODO: remove these lines
    global port
    global hosts
    global actionsets

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

def process_actions():
    global port
    global hosts
    global actionsets

    
    for actionset in actionsets:
        continue_actionsets = True
        shit = False
        connections = []
        
        for action in actionset:
            curraction = action[0]

            if (action[0][:7] == "remote-"):
                curraction = curraction[7:]
                print("Remotely executing " + curraction)
                # TODO: confirm if this is true
                sock = socket.socket()
                sock.connect(('localhost', int(port)))
            else:
                #curraction = curraction[]
                print("Executing " + curraction)
                sockinfo = quote_servers()
                print(sockinfo)
                sock = socket.socket()
                sock.connect(sockinfo)


            message = "action," + curraction
            sock.send(message.encode())
            connections.append(sock)

        while connections:
            ready, empty, error = select.select(connections, [], connections)
            for sock in ready:
                data = sock.recv(1024)
                msg = data.decode()
                status = subprocess.getstatusoutput(msg)
                if data and (status[0] != 0) and (continue_actionsets == True):
                    data = data.decode()
                    print(data)
                    sock.close()
                    connections.remove(sock)
                if (status[0] == 0):
                    shit = True
                    sock.close()
                    connections.remove(sock)
        if(shit):
            continue_actionsets = False
        if continue_actionsets == False:
            break        
          
parse_rakefile()
process_actions()

# print(port)
# print(hosts)
# print(actionsets)

#remote = False
#for actionset in actionsets:
    #for action in actionset:

        #sock = quote_servers()
        #curraction = action[0]

        #if (action[0][:7] == "remote-"):
            #remote = True
            #curraction = curraction[7:]
            #print("Remotely executing " + action[0][7:])
        #else:
            #TODO: remove pass?
            #pass
            #print("Executing " + action[0])

        #command = Popen(curraction, shell=True, stdout=PIPE)
        #output = command.communicate()[0].decode('UTF-8')
        #if (output != ''): print(output)

##-----------------------------------------------------------------------------------------------------------------

# def send_message(socket, message):
#     print("Sending message from client...")
#     socket.send(message.encode())
#     data = socket.recv(1024)
#     data = data.decode()
#     print("INCOMING DATA: ", data)
