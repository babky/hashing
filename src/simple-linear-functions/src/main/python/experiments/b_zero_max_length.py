def compute_chains(a, b, S, p):
    T = {}
    for x in S:
        y = (a * x + b) % p % len(S)
        if y not in T:
            T[y] = set([x])
        else:
            T[y].add(x)

    return T

def compute_differences(Y):
    l = list(Y)
    l.sort()
    first = True
    d = set()
    for x in l:
        if first:
            px = x
            first = False
        else:
            d.add(x - px)
            px = x
    return d

def print_table(T, function = None):
    mcl = max_chain_length(T)
    print "ZERO LENGTH", len(T[0]), "MAX LENGTH", mcl
    if len(T[0]) > 1 and len(T[0]) < mcl:
        print "ASSUMPTION INCR SEQ 0 MAX FAILED"
        quit()

    for k in T:
        l = list(T[k])
        l.sort()

        if function is not None:
            a = function['a']
            b = function['b']
            p = function['p']
            m = function['m']

            print "CHAIN", k
            for x in l:
                y = (a * x + b) % p
                print "\t ELEMENT", x, "%d + %d * m" % (y % m, y / m) 
        else:
            d = compute_differences(T[k])
            print k, len(d), l
            if len(d) > 1:
                print "ASSUMPTION II FAILED"
                quit()
            

def max_chain_length(T):
    m = 0
    for k in T:
        if m < len(T[k]):
            m = len(T[k])

    return m

if __name__ == "__main__":
    p = 24767
    m = 64

    for a in range(p):
        T = compute_chains(a, 0, range(m), p)   
        print "NEW FUNCTION", a
        print_table(T, {'a': a, 'b': 0, 'p': p, 'm': m})
        print

