import math
b = 30 # 14400
p = 14461 #2161 
m = 8

S = [
    [0, 1, m, 2 * m],
    [0, 1, m, m + 1],
    [0, 1, 2, 3]
]

def h(x, a, b):
    return (x * a + b) % p % m

def count(S):
    cnt = 0
    for b in range(p):
        for a in range(p):
            y = h(S[0], a, b)
            yes = True
            for x in S:
                if h(x, a, b) != y:
                    yes = False
            if yes:
                cnt += 1
                # print("a: {0}, corr-1: {1}, corr-2: {2}".format(a, math.floor((a + b) / p), math.floor((d * a + b) / p)))
    return cnt

for s in S:
    print s, "\t", count(s)

