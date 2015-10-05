import math
b = 30 # 14400
p = 2161
m = 8
d = 2160

def h(x, a, b):
    return (x * a + b) % p % m

for a in range(p):
    if h(0, a, b) == h(1, a, b) and h(0, a, b) == h(d, a, b):
        print("a: {0}, corr-1: {1}, corr-2: {2}".format(a, math.floor((a + b) / p), math.floor((d * a + b) / p)))

