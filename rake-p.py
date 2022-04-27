# CITS3002 2022 Sem 1 - Project
# authors
#   - Daivik Anil 22987816
#   - Nathan Eden 22960674
#   - Reiden Rufin 22986337

rakefile_array = []
with open('Rakefile', 'r') as f:
    for line in f:
        if "#" in line: #skip comments
            continue
        rakefile_array.append(line)


print(rakefile_array)



