from universal_classes.system import UniversalSystem

class NonCollidingMultiplicativeSystem(UniversalSystem):
    
    def __init__(self, p, m):
        UniversalSystem.__init__(self)
        self.p = p
        self.m = m
        
        self.functions = set(range(self.p / 2 + 1, self.p));
        for i in range(1, self.p / self.m):
            k  = i * self.m;
            if k in self.functions:
                self.functions.remove(k)
                
            k  = ((p - i) * self.m) % self.p;
            if k in self.functions:
                self.functions.remove(k)
            
        self.all_functions_count = len(self.functions)
        
    def restrict_collision_functions(self, x):
        UniversalSystem._restrict_collision_functions_set(self, x)
        
    def compute_function(self, function, x):
#        return (((function * x) % self.p) % self.m, math.floor(abs(math.sin(function * x) * self.m))) 
        return ((((function) * x) % self.p) % self.m, ((((x + 1) * (x + 2)) % self.p)) % self.p % self.m)
        
    def get_all_function_count(self): 
        return self.all_functions_count
