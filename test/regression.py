import unittest
import os
from itertools import product
from polyleven import levenshtein


class TestEnumeratePatterns(unittest.TestCase):

    def setUp(self):
        basedir = os.path.dirname(__file__)
        self.fp = open(os.path.join(basedir, 'data/regress.txt'))

    def tearDown(self):
        self.fp.close()

    def parseline(self, line):
        dist, s1, s2 = line.strip().split('|')
        return int(dist), s1, s2

    def test_basic(self):
        for line in self.fp:
            dist, s1, s2 = self.parseline(line)

            with self.subTest(s1=s1,s2=s2):
                self.assertEqual(dist, levenshtein(s1, s2))

    def test_with_upperbound(self):
        for k in range(5):
            for line in self.fp:
                dist, s1, s2 = self.parseline(line)

                with self.subTest(k=k,s1=s1,s2=s2):
                    self.assertEqual(min(dist, k+1), levenshtein(s1, s2, k))

if __name__ == '__main__':
    unittest.main(verbosity=2)
