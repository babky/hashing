import random
import argparse
from lib import lbin

def choose_generator(generator_size, p):
    G = []
    for i in range(generator_size):
        a = random.randint(1, p - 1)
        G.append(a)
    return G

def multiply_set(D, x):
    return [x * d for d in D]

def shift_set(S, b, p):
    return [(x + b) % p for x in S]

def generate_set(G, l, m, p):
    D = range(l)
    S = []

    for x in G:
        S0 = multiply_set(D, x)
        S += S0
    return set(S)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Possibly generate an unsuitable set.')
    parser.add_argument('-p', metavar='p', default=15485863, type=int, help='Prime.')
    parser.add_argument('-m', metavar='m', default=2 ** 16, type=int, help='Table size.')
    parser.add_argument('-r', metavar='r', default=1024, type=int, help='Repeats.')
    args = vars(parser.parse_args())

    p = args['p']
    m = args['m']
    l = int(m ** 0.25)
    S = []
    G = choose_generator(m / l, p)
    G2 = choose_generator(m / l / l, p)
    j = 0
    for g2 in G2:
        for i in range(l):
            S += shift_set(generate_set([G[j]], l, m, p), g2 * i, p)
            j += 1
    S = set(S)
    print p, m, len(S)

    sl = 0
    for i in range(args['r']):
        l = lbin(S, random.randint(1, p - 1), 0, p, m)
        sl += l
        # print l

    print float(sl) / args['r']

