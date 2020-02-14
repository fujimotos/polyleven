##### LIST OF LIBRARIES IN THIS SCIPRT ####################
#
# edlib        https://github.com/Martinsos/edlib
# distance     https://github.com/doukremt/distance
# jellyfish    https://github.com/jamesturk/jellyfish
# editdistance https://github.com/aflc/editdistance
# Levenshtein  https://github.com/ztane/python-Levenshtein
# polyleven    https://github.com/fujimotos/polyleven
#
###########################################################

import importlib
import os.path
import sys
import random
from timeit import timeit

LIBRARIES = (
    'edlib.align',
    'editdistance.eval',
    'jellyfish.levenshtein_distance',
    'distance.levenshtein',
    'Levenshtein.distance',
    'polyleven.levenshtein',
)

POLYLEVEN = 'polyleven.levenshtein'


WORD_TXT = 'data/words.txt'

def load_word():
    basedir = os.path.dirname(sys.argv[0])
    path = os.path.join(basedir, WORD_TXT)

    with open(path, encoding="utf-8") as fp:
        return [x.strip() for x in fp]

def load_func(target):
    modname, funcname = target.split('.', maxsplit=1)

    module = importlib.import_module(modname)
    return getattr(module, funcname)

def get_platform():
    import platform
    uname = platform.uname()
    pyver = platform.python_version()
    return 'Python %s on %s (%s)' % (pyver, uname.system, uname.machine)

def benchmark():
    words = load_word()
    sample = random.sample(words, 10)
    total = len(words) * len(sample)

    print('System:', get_platform())
    print('Words :', len(words))
    print('Sample:', len(sample))
    print('Total : %s calls\n' % total)

    print('%-30s %-15s %s' % ('#', 'TIME[sec]', 'SPEED[calls/s]'))

    def wrap(f, *args):
        def func():
            return len([f(x, y, *args) for x in sample for y in words])
        return func

    for target in LIBRARIES:
        try:
            func = load_func(target)
        except (AttributeError, ImportError):
            print('%-30s %-15.3f %i' % (target + ' (not found)', -1, -1))
            continue

        sec = timeit('func()', globals={'func': wrap(func)}, number=1)
        calls = total / sec
        print('%-30s %-15.3f %i' % (target, sec, calls))

    for k in range(3, 0, -1):
        func = load_func(POLYLEVEN)
        sec = timeit('func()', globals={'func': wrap(func,k)}, number=1)
        calls = total / sec
        print('%-30s %-15.3f %i' % (target + ' (k=%s)' % k, sec, calls))

if __name__ == '__main__':
    benchmark()
