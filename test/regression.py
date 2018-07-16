import unittest
import os
import random
import binascii
import string
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

    def get_random_string(self, minlen, maxlen):
        n = random.randint(minlen, maxlen)
        b = os.urandom(n)
        return ''.join(string.printable[i & 0x3f] for i in b)

    def test_short(self):
        for i in range(2000):
            s1 = self.get_random_string(0, 16)
            s2 = self.get_random_string(0, 16)
            with self.subTest(s1=s1, s2=s2):
                self.assertEqual(
                    _debug(WAGNER_FISCHER, s1, s2),
                    _debug(MYERS1999, s1, s2)
                )

    def test_medium(self):
        for i in range(1000):
            s1 = self.get_random_string(128, 256)
            s2 = self.get_random_string(128, 256)
            with self.subTest(s1=s1, s2=s2):
                self.assertEqual(
                    _debug(WAGNER_FISCHER, s1, s2),
                    _debug(MYERS1999, s1, s2)
                )

    def test_long(self):
        for i in range(100):
            s1 = self.get_random_string(512, 1024)
            s2 = self.get_random_string(512, 1024)
            with self.subTest(s1=s1, s2=s2):
                self.assertEqual(
                    _debug(WAGNER_FISCHER, s1, s2),
                    _debug(MYERS1999, s1, s2)
                )

class TestUnicode(unittest.TestCase):

    KANAHIRA = "".join(chr(i) for i in range(0x3000, 0x3100))

    def get_random_string(self, minlen, maxlen):
        n = random.randint(minlen, maxlen)
        b = os.urandom(n)
        return ''.join(self.KANAHIRA[i & 0xff] for i in b)

    def test_unicode_short(self):
        for i in range(2000):
            s1 = self.get_random_string(0, 16)
            s2 = self.get_random_string(0, 16)
            with self.subTest(s1=s1, s2=s2):
                self.assertEqual(
                    _debug(WAGNER_FISCHER, s1, s2),
                    _debug(MYERS1999, s1, s2)
                )

    def test_unicode_medium(self):
        for i in range(1000):
            s1 = self.get_random_string(128, 256)
            s2 = self.get_random_string(128, 256)
            with self.subTest(s1=s1, s2=s2):
                self.assertEqual(
                    _debug(WAGNER_FISCHER, s1, s2),
                    _debug(MYERS1999, s1, s2)
                )

if __name__ == '__main__':
    unittest.main(verbosity=2)
