p = 13835137

for m in [2 ** (2 * (x + 1)) for x in range(12)]:
    M = []

    # Set is a bad set.
    S = []
    sm = int(m ** 0.5)
    for JJ in range(sm):
        J = JJ + 2
        L = []
        l = 0
        T = range(m * sm * 4)
        for x in T:
            if x % J == 0:
                l -= 1
            l += 1
            L.append(l)
        
        T = map(lambda x: 1 + x, T)
        cnt = 0
        last_l = 0
        i = 0
        while cnt < sm:
            if last_l != L[i] and L[i] % m == 1:
                cnt += 1
                S.append(T[i])
                last_l = L[i]
            i += 1
                
    S = set(S)
        
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
        
    print m, sum(M) / float(len(M))
            
