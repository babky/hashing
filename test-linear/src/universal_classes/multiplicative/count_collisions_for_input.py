from universal_classes.multiplicative.system import MultiplicativeSystem
from universal_classes.multiplicative.pair_system import PairMultiplicativeSystem
from universal_classes.multiplicative.non_colliding_system import NonCollidingMultiplicativeSystem

if __name__ == "__main__":
    p = 7817
    m = 23
    
    s = NonCollidingMultiplicativeSystem(p, m)
#    s = MultiplicativeSystem(p, m)
    
#    s.restrict_collision_functions(1 + 23 * 0)
#    s.restrict_collision_functions(1 + 23 * 1)
#    s.restrict_collision_functions(1 + 23 * 2)
#    s.restrict_collision_functions(1 + 23 * 3)
#    s.restrict_collision_functions(1 + 23 * 4)
#    s.restrict_collision_functions(1 + 23 * 5)
#    s.restrict_collision_functions(1 + 23 * 6)
#    s.restrict_collision_functions(1 + 23 * 7)
#    s.restrict_collision_functions(1 + 23 * 8)
#    s.restrict_collision_functions(1 + 23 * 9)

    s.restrict_collision_functions(1)
    s.restrict_collision_functions(2)
    s.restrict_collision_functions(3)
    s.restrict_collision_functions(4)
    s.restrict_collision_functions(5)
    s.restrict_collision_functions(6)

    print s
#    for f in s.get_colliding_functions():
#        print "f = {2}; f(1) = {0}; f(2) = {1}".format(s.compute_function(f, 1), s.compute_function(f, 2), f)
