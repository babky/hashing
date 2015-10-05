from b_zero_max_length import compute_chains, compute_differences, print_table

if __name__ == "__main__":
    m = 1033
    p = 24767
    print "We will go up to", p

    S = range(m)
    for a in range(p):
        if a % 1000 == 0:
            print a

        T = compute_chains(a, 0, S, p)
        D = set()
        for c in T:
            d = compute_differences(T[c])
            if len(d) > 1:
                print "ASSUMTPION FAILED - SINGLE CHAIN"
                print a
                print_table(T)
                quit()
            D.union(d)

        if len(D) > 1:
            print "ASSUMPTION FAILED - CROSS CHAIN"
            print a
            print_table(T)
            quit()

    print "IT WORKS FINE"
