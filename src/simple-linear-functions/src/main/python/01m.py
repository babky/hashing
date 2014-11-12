import math
b = 30 # 14400
p = 14461 #2161 
m = 8
d = 14459

# p = 81223 
# m = 8
# d = p - 20000

p = 2161
m = 8
d = 500

def h(x, a, b):
        return (x * a + b) % p % m

if __name__ == "__main__":
        cnt = 0
        for b in range(p):
            for a in range(p):
                if h(0, a, b) == h(1, a, b) and h(0, a, b) == h(d, a, b):
                    cnt += 1
                    # print("a: {0:7d}, b: {3:7d}; corr-1: {1:7d}, corr-2: {2:7d}".format(a, int(math.floor((a + b) / p)), int(math.floor((d * a + b) / p)), b))

        print("Count: {0}".format(cnt))
        print("Estim 1: {0}".format(p * p / m / d + 2 * p * p / m / m))
        print("Estim 2: {0}".format(d * (d + 1) / 2 / m / (p - d) + p * p / m / m))

