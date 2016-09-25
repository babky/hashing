import math
import sys

p = 14461
p = 2161
m = 17
d = 8

def h(x, a, b):
    return (x * a + b) % p % m

cnt = 0
same_cnt = 0
for b in range(p):
    for a in range(p):
        same = (h(0, a, b) == h(1, a, b) and h(0, a, b) == h(d, a, b))
        collide = True
        y = h(0, a, b)
        for x in range(d + 1):
            if h(x, a, b) != y:
                collide = False
        if same and not collide:
            print("Not collide and same for a = {0}, b = {1}.".format(a, b))
            for x in range(d + 1):
                print("x: {0}\th(x): {1}\t leaps: {2}".format(x, h(x, a, b), (a * x + b) / p))

        if same:
            same_cnt += 1
        if collide:
            for x in range(d + 1):
                    print("x: {0:7d}\th(x): {1:7d}\t leaps: {2}\t({3}, {4})".format(x, h(x, a, b), (a * x + b) / p, a, b))

            cnt += 1

print cnt
print same_cnt
print p ** 2 / m / d + p ** 2 / m / m + p

# print("a: {0}, corr-1: {1}, corr-2: {2}".format(a, math.floor((a + b) / p), math.floor((d * a + b) / p)))

