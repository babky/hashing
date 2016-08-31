from numpy.lib.scimath import log2

C_EPSILON = 17.31
EPSILON = 0.8967
MULTIPLICATIVE_CONSTANT = 4

def get_estimate_for_k(k, ub = 8192, lb = 64):
    m = C_EPSILON * MULTIPLICATIVE_CONSTANT * k
    while True:
        mb = (ub + lb) / 2
        sl = log2(mb) * log2(log2(mb)) * m
        if abs(sl - mb) < 1:
            return ub
        
        if sl > mb:
            lb = mb
        else:
            ub = mb
            
def get_the_best_n():
    ub = 4.0
    lb = 3.0
    
    while ub - lb > 0.01:
        mb = (ub + lb) / 2
        if is_estimate_valid(1 / mb):
            lb = mb
        else:
            ub = mb
                    
    return lb
            
def is_estimate_valid(k):
    n = get_estimate_for_k(k)
    return (1.0 / (k * log2(n)))**(log2(log2(n))) <= (1 - EPSILON) / 2 

if __name__ == "__main__":
    print get_estimate_for_k(0.2766)
    print is_estimate_valid(0.2766)
    print get_the_best_n()
