import unittest
import os
import random
import binascii
from itertools import product
from Levenshtein import distance
from polyleven import levenshtein

class TestRandomPattern(unittest.TestCase):

    def getstr(self):
        r = os.urandom(random.randint(0, 512))
        return binascii.hexlify(r).decode()

    def test_simple(self):
        for i in range(2048):
            s1, s2 = self.getstr(), self.getstr()
            with self.subTest(s1=s1, s2=s2):
                self.assertEqual(distance(s1, s2), levenshtein(s1, s2))

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
