import unittest
import os

from polyleven import levenshtein

def load_data(path):
    res = []
    basedir = os.path.dirname(__file__)
    with open(os.path.join(basedir, 'data', path)) as fp:
        for line in fp:
            dist, s1, s2 = line.strip().split(',')
            res.append((int(dist), s1, s2))
    return tuple(res)

TEST_ASCII = load_data('regress_ascii.csv')
TEST_LONG = load_data('regress_long.csv')
TEST_UNICODE = load_data('regress_unicode.csv')

class TestPattern(unittest.TestCase):

    def test_ascii(self):
        for (dist, s1, s2) in TEST_ASCII:
            with self.subTest(s1=s1, s2=s2):
                self.assertEqual(dist, levenshtein(s1, s2))

    def test_ascii_with_k(self):
        for k in (0, 1, 2, 3):
            for (dist, s1, s2) in TEST_ASCII:
                with self.subTest(k=k, s1=s1, s2=s2):
                    self.assertEqual(min(dist, k+1), levenshtein(s1, s2, k))

    def test_long(self):
        for (dist, s1, s2) in TEST_LONG:
            with self.subTest(s1=s1, s2=s2):
                self.assertEqual(dist, levenshtein(s1, s2))

    def test_unicode(self):
        for (dist, s1, s2) in TEST_UNICODE:
            with self.subTest(s1=s1, s2=s2):
                self.assertEqual(dist, levenshtein(s1, s2))

    def test_unicode_with_k(self):
        for k in (0, 1, 2, 3):
            for (dist, s1, s2) in TEST_UNICODE:
                with self.subTest(k=k, s1=s1, s2=s2):
                    self.assertEqual(min(dist, k+1), levenshtein(s1, s2, k))

if __name__ == '__main__':
    unittest.main(verbosity=2)
