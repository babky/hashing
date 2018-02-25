import random
from collections import namedtuple
from enum import Enum

from typing import List, Tuple


def contains_collision(beta: List[int]) -> bool:
    """
    Returns true if the list beta contains a pair of repeated items.
    """
    return len(set(beta)) < len(beta)


def contains_z_collision(beta: List[int], omitted_occurrences: List[int]) -> bool:
    """
    Checks if the list beta contains an element from ``omitted_occurrences`` or contains a collision.
    """
    set_beta = set(beta)
    return (len(set(omitted_occurrences) & set_beta) > 0) or (len(set_beta) < len(beta))


def random_list(n: int, count: int) -> List[int]:
    """
    Returns a list of ``count`` random integers from {0, 1, 2, 3, ..., n - 1}.
    """
    return list(map(lambda _: random.randint(0, n - 1), range(count)))


def omit_first_occurrences(items: List[int], to_omit: List[int]) -> List[int]:
    """
    Removes the first occurrence of each element from ``to_omit`` from ``items``.
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
    l_parameter = int(0.5 * (n ** 0.5))
    c = 0
    s = 0

    for _ in range(r):
        items = random_list(n, l_parameter)
        if 0 not in items or 1 not in items:
            continue

        s += 1
        items = omit_first_occurrences(items, [0, 1])
        if contains_z_collision(items, [0, 1]):
            c += 1

    return c, s, 1 - c / s


def compute(p: int, h: List[int], n: int, s: int) -> int:
    y = 0
    for a in h:
        y = ((y * s) + a) % p
    return y % n


def generate_walk(p: int, h: List[int], n: int, s: int) -> Tuple[List[int], int]:
    w = []
    while True:
        if s in w:
            return w, s

        w.append(s)
        s = compute(p, h, n, s)


def generate_function(k: int, p: int) -> List[int]:
    coefficients = []
    for _ in range(k):
        coefficients.append(random.randint(0, p - 1))
    h = coefficients
    return h


class WalkResult(Enum):
    NOT_UNIQUE = 0
    PATH_NOT_TRAVERSED = 1
    PATH_TRAVERSED = 2


WalkPoint = namedtuple('WalkPoint', ['source', 'destination'])


def check_walk_k_independence(k: int, p: int, n: int, path: List[WalkPoint]) -> WalkResult:
    h = generate_function(k, p)
    s = random.randint(0, n - 1)
    w = generate_walk(p, h, n, s)[0]
    for point in path:
        if len(w) <= point.source:
            return WalkResult.NOT_UNIQUE

        if w[point.source] != point.destination:
            return WalkResult.PATH_NOT_TRAVERSED

    return WalkResult.PATH_TRAVERSED


def check_walk_inclusion(k: int, p: int, n: int, incl: List[int]) -> bool:
    h = generate_function(k, p)
    s = random.randint(0, n - 1)
    w = generate_walk(p, h, n, s)[0]
    return set(incl).issubset(set(w))


WalkIndependenceTestResult = namedtuple(
    'WalkIndependenceTestResult',
    [
        'traversed',
        'not_traversed',
        'not_unique',
        'count'
    ]
)


def check_walk_k_independence_repeated(k: int,
                                       p: int,
                                       n: int,
                                       path: List[WalkPoint],
                                       repetitions: int) -> WalkIndependenceTestResult:
    traversed = 0
    not_traversed = 0
    not_unique = 0
    count = 0

    for _ in range(repetitions):
        single_result = check_walk_k_independence(k, p, n, path)
        count += 1

        if single_result == WalkResult.NOT_UNIQUE:
            not_unique += 1
            not_traversed += 1
        elif single_result == WalkResult.PATH_NOT_TRAVERSED:
            not_traversed += 1
        else:
            traversed += 1

    return WalkIndependenceTestResult(
        traversed=traversed,
        not_traversed=not_traversed,
        not_unique=not_unique,
        count=count
    )


def check_walk_inclusion_repeated(k: int, p: int, n: int, incl: List[int], r: int) -> Tuple[int, int, float]:
    count = 0
    for _ in range(r):
        if check_walk_inclusion(k, p, n, incl):
            count += 1
    return count, r, count / r


def run_walk_k_independence_test():
    r = 10000000
    p = 59
    n = 16
    k = 4
    path = [
        WalkPoint(5, 1),
        WalkPoint(6, 2),
        WalkPoint(7, 3),
        WalkPoint(4, 4),
    ]

    result = check_walk_k_independence_repeated(k, p, n, path, r)

    print(
        f"""
        Walk k-independence Test Result   
        
        Repeats:        {result.count}
        Not unique:     {result.not_unique}
        Not traversed:  {result.not_traversed}
        Traversed:      {result.traversed}
        Traversed prob: {result.traversed / result.count}
        Target prob:    {n ** -4}
        """.strip())


if __name__ == "__main__":
    random.seed()
    run_walk_k_independence_test()
