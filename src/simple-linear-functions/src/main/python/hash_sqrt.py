MAX = 10000000

def get_primes(m):
    T = sieve = range(m)
    for x in sieve:
        T[x] = True
    T[0] = T[1] = False

    for i in range(m)[2:]:
        if not T[i]:
            continue

        j = i
        while j < m - i:
            j += i
            T[j] = False

    P = []
    p = 1
    for i in range(m):
        if not T[i]:
            continue
        if i >= p * 2:
            P.append(p)
            p = i
    return P

def get_set(p, m, sm):
    S = set()
    l = p / 2
    for aa in [x * p / 2 / sm for x in range(sm)]:
        a = aa + p / 2
        if a % 2 == 1:
            a += 1
        L = []
        for xx in range(m * sm):
            x = xx + 1
            L.append(x * a / p)
    
        s = 0
        for xx in range(m * sm):
            x = xx + 1
            if L[xx] % m == s and x not in S:
                S.add(x)

    return S


P = get_primes(MAX)
print "Primes: ", P

for p in [x for x in P if x > 64]:
    m = int((p / 2) ** 0.5)
    M = []

    # Set is a bad set.
    sm = int(m ** 0.5)
    S = get_set(p, m, sm)

    for aa in range(p / 2):
        a = p - 1 - aa
        if a % 2 == 1:
            continue
        t = {}
        
        for x in S: #range(m)
            # x = 1 + xx * m
            y = a * x % p % m
            if y not in t:
                t[y] = 1
            else:
                t[y] += 1
            
        M.append(max([t[k] for k in t]))
        
    print "{0:8d} {1:8d} {2:16f}" .format(p, m, sum(M) / float(len(M)))
            
