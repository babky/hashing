import random
import argparse
from lib import lbin

def choose_generator(generator_size, p):
    G = []
    for i in range(generator_size):
        x = random.randint(0, p - 1)
        G.append(x)
    return G

def multiply_set(D, x):
    return [x * d for d in D]

def shift_set(S, b, p):
    return [(x + b) % p for x in S]

def generate_set(G, l, m):
    D = range(l)
    S = []
    for x in G:
        S0 = multiply_set(D, x)
        S += shift_set(S0, random.randint(0, m - 1), p)
    return set(shift_set(S, 1, p))
    
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Possibly generate an unsuitable set.')
    parser.add_argument('-p', metavar='p', default=15485863, type=int, help='Prime.')
    parser.add_argument('-m', metavar='m', default=2 ** 16, type=int, help='Table size.')
    parser.add_argument('-r', metavar='r', default=1024, type=int, help='Repeats.')
    args = vars(parser.parse_args())

    p = args['p']
    m = args['m']
    l = int(m ** (float(1) / 4))
    G = choose_generator(m / l, p)
    A = []

    print p, m, l, len(G), len(A) #, len(G) ** 2 / 2 * (1 - 2 * len(G) ** 2 / p)
    S = generate_set(G, l, m)
    print len(S)

    sl = 0
    for i in range(args['r']):
        l = lbin(S, random.randint(1, p - 1), 0, p, m)
        sl += l
        # print l

    print float(sl) / args['r']

