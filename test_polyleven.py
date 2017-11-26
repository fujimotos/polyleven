import unittest
from itertools import product
from polyleven import levenshtein


def wagner_fischer(s, t):
    """Simple implementation of Wagner-Fischer algorithm"""
    n, m = len(s), len(t)

    matrix = {}
    for i in range(n + 1):
        matrix[(i, 0)] = i
    for j in range(m + 1):
        matrix[(0, j)] = j

    for j in range(1, m + 1):
        for i in range(1, n + 1):
            if s[i - 1] == t[j - 1]:
                matrix[(i, j)] = matrix[(i - 1, j - 1)]
            else:
                matrix[(i, j)] = min([
                    matrix[(i - 1, j)],
                    matrix[(i, j - 1)],
                    matrix[(i - 1, j - 1)],
                ]) + 1

    return matrix[(i,j)]


class TestPolyLeven(unittest.TestCase):

    CHARS = ('', 'a', 'b', 'c', 'd')

    PATTERNS = set("".join(s) for s in product(*[CHARS]*4))

    def test_unbounded(self):
        for s1, s2 in product(self.PATTERNS, self.PATTERNS):
            self.assertEqual(levenshtein(s1, s2), wagner_fischer(s1, s2))

    def test_bounded(self):
        for k in range(5):
            for s1, s2 in product(self.PATTERNS, self.PATTERNS):
                self.assertEqual(
                    levenshtein(s1, s2, k),
                    min(wagner_fischer(s1, s2), k+1)
                )

if __name__ == '__main__':
    unittest.main(verbosity=2)
