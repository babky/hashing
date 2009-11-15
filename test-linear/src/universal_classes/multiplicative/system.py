class MultiplicativeSystem:
    
    def __init__(self, p, m):
        self.functions = range(1, p);
        self.p = p
        self.m = m
        self.colliding_elements = []
        
    def compute_function(self, function, x):
        return ((function * x) % self.p) % self.m 
        
    def restrict_collision_functions(self, x):
        if self.colliding_elements == []:
            self.colliding_elements = [x]
            return
        
        y = self.colliding_elements[0]
        max = len(self.colliding_elements)
        i = 0
        while i < max:
            function = self.functions[i]
            if self.compute_function(function, x) != self.compute_function(function, y):
                del self.functions[i]
                max -= 1
            else:
                i += 1
        
        self.colliding_elements.append(x)
        