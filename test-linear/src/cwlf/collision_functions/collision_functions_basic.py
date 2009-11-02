from collision_functions import CollisionFunctions

#cf = CollisionFunctions(643, 64)
#elements = [1, 2, 3, 9, 15, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 43, 44]
#for element in elements:
#    cf.restrict_collision_functions(element)
#    print cf.get_colliding_function_count()

cf = CollisionFunctions(5, 2)

print cf.compute_function((4, 2), 1)
print cf.compute_function((4, 2), 2)
print cf.compute_function((4, 2), 3)

elements = [1, 2, 3]
for element in elements:
    cf.restrict_collision_functions(element)
    print cf.get_colliding_functions()
    print cf.get_colliding_function_count()
