from cwlf.collision_functions.collision_functions import CWLFSystem
import fractions

N = 2099
m = 64

c = N / m
c = fractions.Fraction((c + 1) ** 2, c ** 2)

cf = CWLFSystem(N, m)
elements = [1, 2, 3, 9, 15, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 43, 44]
l = 0
for element in elements:
    cf.restrict_collision_functions(element)
    print "After {0} is added, we've got {1} and we would like to have at max {2} functions.".format(
        element,
        cf.get_colliding_function_count(),
        float(N * N * ((c / m) ** l))
    )
    l += 1
