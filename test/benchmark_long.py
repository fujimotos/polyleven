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
import os
import os.path
import sys
import string
import random
import binascii
from timeit import timeit

LIBRARIES = (
    'edlib.align',
    'editdistance.eval',
    'jellyfish.levenshtein_distance',
    'distance.levenshtein',
    'Levenshtein.distance',
    'polyleven.levenshtein',
)

def load_func(target):
    modname, funcname = target.split('.', maxsplit=1)

    module = importlib.import_module(modname)
    return getattr(module, funcname)

def random_string(size=512):
    b = os.urandom(size)
    return ''.join(string.printable[i % 0x3f] for i in b)

def get_platform():
    import platform
    uname = platform.uname()
    pyver = platform.python_version()
    return 'Python %s on %s (%s)' % (pyver, uname.system, uname.machine)

def gen_inputs(total, sizes):
    res = {}
    for s in sizes:
        res[s] = [(random_string(s), random_string(s)) for x in range(total)]
    return res

def benchmark():
    total = 5000
    sizes = (16, 32, 64, 128, 256, 512, 1024)

    data = gen_inputs(total, sizes)

    print('System: %s' % get_platform())
    print('Sample: %s\n' % total)

    print('%-30s ' % "#", end='')
    for s in sizes:
        print('N=%-6s ' % s, end='')
    print()

    def wrap(f, pairs):
        def func():
            return len([f(x, y) for x, y in pairs])
        return func

    for target in LIBRARIES:
        try:
            func = load_func(target)
        except (AttributeError, ImportError):
            print('%-30s' % (target + ' (not found)'))
            continue

        print('%-30s ' % target, end='')
        for s in sizes:
            sec = timeit('func()', globals={'func': wrap(func, data[s])}, number=1)
            print('%-8.3f ' % sec, end='')
        print()

if __name__ == '__main__':
    benchmark()
