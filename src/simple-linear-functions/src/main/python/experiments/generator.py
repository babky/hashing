import random
import argparse
from lib import lbin

def choose_generator(generator_size, p):
    G = []
    for _ in range(generator_size):
        x = random.randint(0, p - 1)
        G.append(x)
    return G

def generate_multipliers(G, p):
    A = set()
    for x in G:
        for y in G:
            z = (y - x) % p
            if z not in A:
                A.add(z)
    return A

def multiply_set(D, x):
    return [x * d for d in D]

def generate_set(G, l, **kwargs):
    D = range(l)
    if kwargs.get('omitZero'):
        D = D[1:]

    S = []
    for x in G:
        S += multiply_set(D, x)
    return set(S)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Possibly generate an unsuitable set.')
    parser.add_argument('-p', metavar='p', default=15485863, type=int, help='Prime.')
    parser.add_argument('-m', metavar='m', default=2 ** 16, type=int, help='Table size.')
    parser.add_argument('-r', metavar='r', default=1024, type=int, help='Repeats.')
    args = vars(parser.parse_args())

    p = args['p']
    m = args['m']
    l = 4 * int(m ** (float(1) / 3))
    G = choose_generator(m / l, p)
    # A = generate_multipliers(G, p)
    A = []

    print p, m, l, len(G), len(A) #, len(G) ** 2 / 2 * (1 - 2 * len(G) ** 2 / p)
    S = generate_set(G, l)
    print len(S)

    sl = 0
    for i in range(args['r']):
        l = lbin(S, random.randint(1, p - 1), 0, p, m)
        sl += l
        # print l

    print float(sl) / args['r']

