from universal_classes.multiplicative.non_colliding_system import NonCollidingMultiplicativeSystem

class SpreadingMultiplicativeSystem(NonCollidingMultiplicativeSystem):
    
    def __init__(self, p, m):
        NonCollidingMultiplicativeSystem.__init__(self, p, m)
        
    def compute_function(self, function, x, level = 0):
        v = x % self.m
        u = x / self.m
        if u == 0:
            l = 0
        else:
            l = 1
        while u != 0 and u % self.m == 0:
            l += 1
            u /= self.m
            
        return (((u + v) * function) % self.p % self.p, l)