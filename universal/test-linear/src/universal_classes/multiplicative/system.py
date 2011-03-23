from universal_classes.system import UniversalSystem

class MultiplicativeSystem(UniversalSystem):
    
    def __init__(self, p, m):
        UniversalSystem.__init__(self)
        
        self.functions = range(1, p);
        self.p = p
        self.m = m
        
    def restrict_collision_functions(self, x):
        UniversalSystem._restrict_collision_functions_list(self, x)
        
    def compute_function(self, function, x):
        return ((function * x) % self.p) % self.m 
        
    def get_all_function_count(self): 
        return self.p - 1
