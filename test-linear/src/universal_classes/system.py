class UniversalSystem:
    
    def __init__(self):
        self.colliding_elements = []
        
    def _restrict_collision_functions_set(self, x):
        if self.colliding_elements != []:
            y = self.colliding_elements[0]
            delete = []
            
            for function in self.functions:
                if self.compute_function(function, x) != self.compute_function(function, y):
                    delete.append(function)
        
            for function in delete:
                self.functions.remove(function)

        self.colliding_elements.append(x)

    def _restrict_collision_functions_list(self, x):
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
                    
    def get_colliding_functions(self):
        return self.functions
    
    def get_colliding_function_count(self):
        return len(self.functions)

    def get_colliding_element_count(self):
        return len(self.colliding_elements)

    def get_all_function_count(self):
        raise NotImplementedError()
    
    def compute_function(self, function, x):
        raise NotImplementedError()

    def __str__(self):
        return "Restricted for {3} elements: {0}, all functions: {1}, probability: {2}.".format(
            self.get_colliding_function_count(),
            self.get_all_function_count(),
            float(self.get_colliding_function_count()) / self.get_all_function_count(),
            self.get_colliding_element_count()
        )
