import random

from typing import List, Tuple

def contains_collision(beta: List[int]) -> bool:
    """
    Returns true if the list beta contains a pair of repeated items.
    """
    return len(set(beta)) < len(beta)

def contains_z_collision(beta: List[int], omitted_occurences: List[int]) -> bool:
    """
    Checks if the list beta contains an element from omitted_occurences or contains a collision.
    """
    set_beta = set(beta)
    return (len(set(omitted_occurences) & set_beta) > 0) or (len(set_beta) < len(beta))

def random_list(n: int, L: int) -> List[int]:
    """
    Returns a list of L random integers from {0, 1, 2, 3, ..., n - 1}.
    """
    return list(map(lambda _: random.randint(0, n - 1), range(L)))

def omit_first_occurences(items: List[int], to_omit: List[int]) -> List[int]:
    """
    Removes the first occurence of each element from to_omit from items.
    """
    for i, e in enumerate(items):
        if e in to_omit:
            items.pop(i)
            to_omit.remove(e)

    return items

def verify_walk_contains_two_elements(n: int, r: int) -> Tuple[int, int, float]:
    """
    Computes the probability that the walk contains two elements.
    """
    L = int(0.5 * (n ** 0.5))
    c = 0
    s = 0

    for _ in range(r):
        items = random_list(n, L)
        if 0 not in items or 1 not in items:
            continue

        s += 1
        items = omit_first_occurences(items, [0, 1])
        if contains_z_collision(items, [0, 1]):
            c += 1

    return (c, s, 1 - c / s)

def compute(p: int, h: List[int], n: int, s: int) -> int:
    y = 0
    for a in h:
        y = ((y * s) + a) % p
    return y % n

def generate_walk(p: int, h: List[int], n: int, s: int) -> int:
    w = []
    while True:
        if s in w:
            return w, s

        w.append(s)
        s = compute(p, h, n, s)

def generate_function(k: int, p: int)-> List[int]:
    coefficients = []
    for _ in range(k):
        coefficients.append(random.randint(0, p - 1))
    h = coefficients
    return h

def check_walk_k_independence(k: int, p: int, n: int, K: List[Tuple[int, int]]) -> bool:
    h = generate_function(k, p)
    s = random.randint(0, n - 1)
    w = generate_walk(p, h, n, s)[0]
    for e in K:
        if len(w) <= e[0]:
            return False

        if w[e[0]] != e[1]:
            return False

    return True

def check_walk_inclusion(k: int, p: int, n: int, incl: List[int]) -> bool:
    h = generate_function(k, p)
    s = random.randint(0, n - 1)
    w = generate_walk(p, h, n, s)[0]
    return set(incl).issubset(set(w))

def check_walk_k_independence_repeated(k: int, p: int, n: int, K: List[Tuple[int, int]], r: int) -> Tuple[int, int, float]:
    sum = 0
    for _ in range(r):
        if check_walk_k_independence(k, p, n, K):
            sum += 1
    return (sum, r, sum / r)

def check_walk_inclusion_repeated(k: int, p: int, n: int, K: List[int], r: int) -> Tuple[int, int, float]:
    sum = 0
    for _ in range(r):
        if check_walk_inclusion(k, p, n, K):
            sum += 1
    return (sum, r, sum / r)

if __name__ == "__main__":
    random.seed()

    # n = 11500
    # r = 1000000
    # print(verify_walk_contains_two_elements(n, r))

    r = 1000000
    p = 59
    n = 16
    k = 4
    print(check_walk_k_independence_repeated(k, p, n, [[1, 1], [2, 2], [3, 3], [4, 4]], r))
    print(check_walk_inclusion_repeated(k, p, n, [2, 3], r))
