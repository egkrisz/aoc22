data = []

with open('input.txt', 'r') as f:
    for i, line in enumerate(f.readlines()):
        if line.strip():
            data.append((i, int(line)))

# print(data)
n = len(data)
order = data[:]

for idnum, x in order:
    for i in range(n):
        if data[i][0] == idnum:
            break
    for _ in range(abs(x)):
        swap_i = (i + (x//abs(x))) % n
        data[i], data[swap_i] = data[swap_i], data[i]
        i = swap_i

for i in range(n):
    if data[i][1] == 0:
        break
a = data[(i+1000)%n][1]
b = data[(i+2000)%n][1]
c = data[(i+3000)%n][1]

print("part 1:", a+b+c)

exit(0)

key = 811589153
data = [(idnum, key*x) for (idnum, x) in order]

for count in range(10):
    for idnum, x in order:
        for i in range(n):
            if data[i][0] == idnum:
                break
        swaps = data[i][1] % (n-1)
        for _ in range(abs(swaps)):
            swap_i = (i + (swaps//abs(swaps))) % n
            data[i], data[swap_i] = data[swap_i], data[i]
            i = swap_i
    print(count+1, data)

for i in range(n):
    if data[i][1] == 0:
        break
a = data[(i+1000)%n][1]
b = data[(i+2000)%n][1]
c = data[(i+3000)%n][1]

print("part 2:", a+b+c)
