# CITS3002 2022 Sem 1 - Project
# authors
#   - Daivik Anil 22987816
#   - Nathan Eden 22960674
#   - Reiden Rufin 22986337

rakefile_array = []
with open('Rakefile', 'r') as f:
    for line in f:
        split_line = line.split("#", 1)[0]
        if (split_line != ""):
            rakefile_array.append(split_line)
        
print(rakefile_array)



