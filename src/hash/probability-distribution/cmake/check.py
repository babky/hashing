m = 32
u = 1024

for x in range(u):
    cnt = 0
    for aa in range(u / 2):
        a = 2 * aa + 1
        if (a * x % u) /  (u / m) == 0 and a / (u / m) == 0:
            cnt += 1
    print "{0}, {1}".format(x, cnt)
