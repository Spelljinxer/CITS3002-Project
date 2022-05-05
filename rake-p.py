# CITS3002 2022 Sem 1 - Project
# authors
#   - Daivik Anil 22987816
#   - Nathan Eden 22960674
#   - Reiden Rufin 22986337

port = 0
hosts = []
actionsets = []

with open('Rakefile', 'r') as f:
    setnum = -1

    for line in f:
        line = line.split("#", 1)[0].replace('\n','')
        if (line != ""):
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

print(port)
print(hosts)
print(actionsets)
