rakefile_array = []
with open('Rakefile', 'r') as f:
    for line in f:
        if "#" in line: #skip comments
            continue
        rakefile_array.append(line)


print(rakefile_array)



