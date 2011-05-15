from universal_classes.system import UniversalSystem

class CWLFSystem(UniversalSystem):
    
    def __init__(self, p, m, allow_zero = 1):
        UniversalSystem.__init__(self)
        self.p = p
        self.m = m
        self.allow_zero = allow_zero

        if (self.allow_zero):
            self.functions = set([(a, b) for a in range(p) for b in range(p)])
        else:
            self.functions = set([(a, b) for a in range(1, p) for b in range(p)])

    def restrict_collision_functions(self, x):
        UniversalSystem._restrict_collision_functions_set(self, x)

    def get_all_function_count(self):
        if (self.allow_zero):
            return self.p ** 2
        else:
            return self.p * (self.p - 1)
    
    def compute_function(self, function, x):
        (a, b) = function
        return ((a * x + b) % self.p) % self.m
