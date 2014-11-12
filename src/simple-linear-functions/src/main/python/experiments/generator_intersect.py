def begin_intersection_size(s, g, p):
    c = 0
    for x in range(s):
        if x * g % p < s:
            c += 1
    return c

def intersection_estimate_size(s, g, p):
    return (g * s / p + 2) * (s / g + 1)

if __name__ == "__main__":
    s = 647
    g = 5432
    p = 24767
    print begin_intersection_size(s, g, p), intersection_estimate_size(s, g, p)


