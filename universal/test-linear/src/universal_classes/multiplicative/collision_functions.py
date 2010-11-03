from universal_classes.multiplicative.system import MultiplicativeSystem
from universal_classes.multiplicative.pair_system import PairMultiplicativeSystem
from universal_classes.multiplicative.non_colliding_system import NonCollidingMultiplicativeSystem
from universal_classes.multiplicative.spreading_system import SpreadingMultiplicativeSystem

if __name__ == "__main__":
    max = 0
    max_i_j = ()
    p = 2099
    m = 22
    
    s = SpreadingMultiplicativeSystem(p, m)

    for i in range(23, 24):
        print "For i {0}.".format(i)
        for j in range(0, p):
            if j % 100 == 0:
                print "For j {0}.".format(j)
                
            if i == j:
                continue
            
            s = SpreadingMultiplicativeSystem(p, m)
            s.restrict_collision_functions(i)
            s.restrict_collision_functions(j)
            
            if (max < s.get_colliding_function_count()):
                max = s.get_colliding_function_count()
                max_i_j = (i, j)
                print max_i_j
                print s
                
    (i, j) = max_i_j
    s = SpreadingMultiplicativeSystem(p, m)
    s.restrict_collision_functions(i)
    s.restrict_collision_functions(j)
    print s, s.get_colliding_functions()
    for f in s.get_colliding_functions():
        print "f = {2}; f({3}) = {0}; f({4}) = {1}".format(s.compute_function(f, i), s.compute_function(f, j), f, i, j)
