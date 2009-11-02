class CollisionFunctions:
    
    def __init__(self, p, m):
        self.p = p
        self.m = m
        self.functions = set([(a, b) for a in range(p) for b in range(p)])
        self.colliding_elements = []

    def restrict_collision_functions(self, x):
        if self.colliding_elements != []:
            y = self.colliding_elements[0]
            delete = []
            
            for function in self.functions:
                if self.compute_function(function, x) != self.compute_function(function, y):
                    delete.append(function)
        
            for function in delete:
                self.functions.remove(function)

        self.colliding_elements.append(x)
                    
    def get_colliding_functions(self):
        return self.functions
    
    def get_colliding_function_count(self):
        return len(self.functions)

    def get_colliding_element_count(self):
        return len(self.colliding_elements)

    def get_all_function_count(self):
        return self.p ** 2
    
    def compute_function(self, function, x):
        (a, b) = function
        return ((a * x + b) % self.p) % self.m

    def __str__(self):
        return "Restricted for {3} elements: {0}, all functions: {1}, probability: {2}.".format(
            self.get_colliding_function_count(),
            self.get_all_function_count(),
            float(self.get_colliding_function_count()) / self.get_all_function_count(),
            self.get_colliding_element_count()
        )
