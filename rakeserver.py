# CITS3002 2022 Sem 1 - Project - rakeserver
# authors
#   - Daivik Anil 22987816
#   - Nathan Eden 22960674
#   - Reiden Rufin 22986337
#--------------------------------------------------------------------------

import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

port = 6238 #going to need a way to change this so its dynamic with the client's Rakefiles
sock.bind(('', port))
print("Listening on port " + str(port))

sock.listen(5)

while True:
    c, address = sock.accept()
    print("connected from:", address)

    c.send("Hello, client!".encode())
    #receive message from client and decode it
    data = c.recv(1024)
    data = data.decode()
    print("Received:", data)

    c2, address2 = sock.accept()
    print("connected from:", address2)
    data2 = c2.recv(1024)
    data2 = data2.decode()
    print("Received:", data2)


    c.close()
    c2.close()
    break

