import copy
from collision_functions import CollisionFunctions

def count_functions_3(cf, a0, a1, a2):
    cf = copy.deepcopy(cf)

    cf.restrict_collision_functions(a0)
    cf.restrict_collision_functions(a1)
    cf.restrict_collision_functions(a2)

    return cf

def count_functions_2_independently(cf, a0, a1, a2):
    # @type cf1 CollisionFunctions
    cf1 = copy.deepcopy(cf)
    
    # @type cf2 CollisionFunctions
    cf2 = copy.deepcopy(cf)

    cf1.restrict_collision_functions(a0)
    cf1.restrict_collision_functions(a1)

    cf2.restrict_collision_functions(a0)
    cf2.restrict_collision_functions(a2)

    return (cf1, cf2)

#cf = CollisionFunctions(2099, 64, 0)
#cf = CollisionFunctions(109, 64, 0)
#cf = CollisionFunctions(5, 2, 0)

cf = CollisionFunctions(109, 33, 0)
cf_3 = count_functions_3(cf, 1, 2, 3)
(cf_1, cf_2) = count_functions_2_independently(cf, 1, 2, 3)
# @type cf_1 CollisionFunctions
# @type cf_2 CollisionFunctions
# @type cf_3 CollisionFunctions

print cf_3
print cf_2
print cf_1

f = cf_1.functions.pop()
print "One found function: {0}.".format(f)
print "{0}({1}) = {2}".format(f, 1, cf.compute_function(f, 1))
print "{0}({1}) = {2}".format(f, 2, cf.compute_function(f, 2))
print "{0}({1}) = {2}".format(f, 3, cf.compute_function(f, 3))

print "If the independence held we would find {0} functions instead of {1}.".format(cf_1.get_colliding_function_count() * cf_2.get_colliding_function_count() / cf.get_all_function_count(), cf_3.get_colliding_function_count())