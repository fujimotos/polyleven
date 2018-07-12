import unittest
import os
import random
import binascii
from polyleven import _debug, levenshtein
from itertools import product

WAGNER_FISCHER = 0
MBLEVEN = 1
MYERS1999 = 2

class TestEnumeratePatterns(unittest.TestCase):

    def setUp(self):
        basedir = os.path.dirname(__file__)
        self.fp = open(os.path.join(basedir, 'data/regress.txt'))

    def tearDown(self):
        self.fp.close()

    def parseline(self, line):
        dist, s1, s2 = line.strip().split('|')
        return int(dist), s1, s2

    def test_levenshtein(self):
        for line in self.fp:
            dist, s1, s2 = self.parseline(line)

            with self.subTest(s1=s1,s2=s2):
                self.assertEqual(dist, levenshtein(s1, s2))

    def test_with_upperbound(self):
        for k in (0, 1, 2, 3, 4, 5):
            for line in self.fp:
                dist, s1, s2 = self.parseline(line)

                with self.subTest(k=k,s1=s1,s2=s2):
                    self.assertEqual(min(dist, k+1), levenshtein(s1, s2, k))

    def test_wagner_fischer(self):
        for line in self.fp:
            dist, s1, s2 = self.parseline(line)
            with self.subTest(s1=s1, s2=s2):
                self.assertEqual(dist, _debug(WAGNER_FISCHER, s1, s2))

    def test_myers1999(self):
        for line in self.fp:
            dist, s1, s2 = self.parseline(line)
            with self.subTest(s1=s1, s2=s2):
                self.assertEqual(dist, _debug(MYERS1999, s1, s2))

    def test_mbleven(self):
        for k in (1, 2, 3):
            for line in self.fp:
                dist, s1, s2 = self.parseline(line)

                with self.subTest(k=k,s1=s1,s2=s2):
                    self.assertEqual(min(dist, k+1), _debug(MBLEVEN, s1, s2, k))


class TestRandomPattern(unittest.TestCase):

    def get_random_string(self, maxlen=512):
        r = os.urandom(random.randint(0, maxlen))
        return binascii.hexlify(r).decode()

    def test_normal(self):
        for i in range(1024):
            s1 = self.get_random_string()
            s2 = self.get_random_string()
            with self.subTest(s1=s1, s2=s2):
                self.assertEqual(
                    _debug(WAGNER_FISCHER, s1, s2),
                    _debug(MYERS1999, s1, s2)
                )

    def test_long_inputs(self):
        for i in range(5):
            s1 = self.get_random_string(4096)
            s2 = self.get_random_string(4096)
            with self.subTest(s1=s1, s2=s2):
                self.assertEqual(
                    _debug(WAGNER_FISCHER, s1, s2),
                    _debug(MYERS1999, s1, s2)
                )

if __name__ == '__main__':
    unittest.main(verbosity=2)
