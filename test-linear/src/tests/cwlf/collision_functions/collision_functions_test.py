from cwlf.collision_functions.collision_functions import CWLFSystem
import unittest

class CollisionFunctionsTest(unittest.TestCase):

    def test_collision_function_count(self):
        cf = CWLFSystem(17, 13)
        self.assertEqual(cf.compute_function((3, 15), 2), 4, "CWLF function computation test.")

        cf.restrict_collision_functions(1)
        self.assertEqual(17 ** 2, cf.get_colliding_function_count(), "CWLF function count.")

        cf.restrict_collision_functions(2)
        first_count = cf.get_colliding_function_count()
        cf.restrict_collision_functions(2)
        second_count = cf.get_colliding_function_count()
        self.assertEqual(first_count, second_count, "CWLF function count - restriction.")

if __name__ == '__main__':
    unittest.main()

