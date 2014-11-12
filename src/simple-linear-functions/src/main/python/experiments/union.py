'''
Performs a test where we check how the union of the two sets behaves wrt. the expected length of the largest bin.
'''
from experiments.generator import choose_generator, generate_set
from experiments.lib import lbin
import random

if __name__ == "__main__":
    m2 = 65536
    p = 15485863
    BASE_SET_COUNT = 128
    m = BASE_SET_COUNT * m2
    G = choose_generator(BASE_SET_COUNT, p)
    sets = []
    for x in G:
        sets.append(set([e % p for e in generate_set([x], m2 + 1, omitZero = True)]))

    a = random.randint(0, p - 1)
    b = random.randint(0, p - 1)
    while len(sets) > 1:
        new_sets = []
        while len(sets) > 1:
            MS = []
            A = sets.pop()
            B = sets.pop()
            la = lbin(A, a, b, p, m)
            lb = lbin(B, a, b, p, m)
            MS.extend(A)
            MS.extend(B)
            MS = set(MS)
            lms = lbin(MS, a, b, p, m)
            new_sets.append(MS)
            print len(A), len(B), "->", la, lb, lms, len(MS), "=", 2 * float(lms) / (la + lb)
        sets = new_sets
        print "NEXT ROUND"

