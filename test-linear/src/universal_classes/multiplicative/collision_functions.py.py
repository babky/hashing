from universal_classes.multiplicative.system import MultiplicativeSystem

if __name__ == "__main__":
#    for i in range(23):
#        for j in range(23):
#            if i == j:
#                continue
#            
#            s = MultiplicativeSystem(23, 7)
#            s.restrict_collision_functions(i)
#            s.restrict_collision_functions(j)
#            
#            print s
    
    s = MultiplicativeSystem(7817, 7817)
    
    s.restrict_collision_functions(1)
    s.restrict_collision_functions(2)
    s.restrict_collision_functions(3)
    s.restrict_collision_functions(4)
    s.restrict_collision_functions(5)
    s.restrict_collision_functions(6)
    s.restrict_collision_functions(7)
    s.restrict_collision_functions(8)
    s.restrict_collision_functions(9)
    s.restrict_collision_functions(10)
    s.restrict_collision_functions(11)
    s.restrict_collision_functions(12)
    s.restrict_collision_functions(13)
    s.restrict_collision_functions(14)
    s.restrict_collision_functions(15)
    s.restrict_collision_functions(16)
    s.restrict_collision_functions(17)
    s.restrict_collision_functions(18)
    s.restrict_collision_functions(19)
    s.restrict_collision_functions(20)
    s.restrict_collision_functions(21)
    
    print s
    for f in s.get_colliding_functions():
        print "f = {2}; f(1) = {0}; f(2) = {1}".format(s.compute_function(f, 1), s.compute_function(f, 2), f)
