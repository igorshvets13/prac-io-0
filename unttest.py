import unittest
import numpy as np
from func import nash_equilibrium

class TestNashEquilibrium(unittest.TestCase):
    def test_nash_equilibrium_case1(self):
        A = np.array([[33, 9, 55],[-1, 3, 3],[2, 2, -7]])
        result = nash_equilibrium(A)
        expected_result = {
            'first player strategy': [1., 0., 0.],
            'second player strategy': [0., 1., 0.],
            'result': [9.]
        }
        self.assertDictEqual(result, expected_result)

    def test_nash_equilibrium_case2(self):
        A = np.array([[3, 2, 4], [1, 4, 2], [2, 1, 5]])
        result = nash_equilibrium(A)
        expected_result = {
            'first player strategy': [0.75, 0.25, 0.  ],
            'second player strategy': [0.5, 0.5, 0. ],
            'result': [2.5]
        }
        self.assertDictEqual(result, expected_result)

    def test_nash_equilibrium_case3(self):
        A = np.array([[1, 1], [0, 0]])
        result = nash_equilibrium(A)
        expected_result = {
            'first player strategy': [1., 0.],
            'second player strategy': [1., 0.],
            'result': [1.]
        }
        self.assertDictEqual(result, expected_result)

if __name__ == '__main__':
    unittest.main()

