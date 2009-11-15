from universal_classes.cwlf.multiplicative.system import MultiplicativeSystem

if __name__ == "main":
    s = MultiplicativeSystem(23, 7)
    s.restrict_collision_functions(1)
    s.restrict_collision_functions(2)
    
    