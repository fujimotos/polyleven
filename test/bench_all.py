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

import time
import edlib
import distance
import jellyfish
import editdistance
import Levenshtein
import polyleven

def do_bench(words, sample, name, func, args=None):
    if args is None:
        args = tuple()

    total = len(words) * len(sample)

    t0 = time.time()
    for w1 in words:
        for w2 in sample:
            func(w1, w2, *args)
    t1 = time.time()

    sec = t1 - t0
    speed = int(1 / (sec / total))

    print('%-30s %-15.3f %i' % (name, sec, speed))


if __name__ == '__main__':
    import os.path
    import sys
    import random
    import platform

    basedir = os.path.dirname(sys.argv[0])
    path = os.path.join(basedir, 'data/words.txt')

    with open(path) as fp:
        words = [x.strip() for x in fp]
    sample = random.sample(words, 10)

    def bench(name, func, args=None):
        return do_bench(words, sample, name, func, args)

    uname = platform.uname()
    pyversion = platform.python_version()

    print('System: Python %s on %s (%s)' % (pyversion, uname.system, uname.machine))
    print('Words :', len(words))
    print('Sample:', len(sample))
    print('Total : %s calls\n' % (len(words) * len(sample)))

    print('%-30s %-15s %s' % ('#', 'TIME[sec]', 'SPEED[calls/s]'))

    bench('edlib.align', edlib.align)
    bench('editdistance.eval', editdistance.eval)
    bench('jellyfish.levenshtein_distance', jellyfish.levenshtein_distance)
    bench('distance.levenshtein', distance.levenshtein)
    bench('polyleven.levenshtein', polyleven.levenshtein)
    bench('Levenshtein.distance', Levenshtein.distance)

    bench('polyleven.levenshtein (k=3)', polyleven.levenshtein, (3,))
    bench('polyleven.levenshtein (k=2)', polyleven.levenshtein, (2,))
    bench('polyleven.levenshtein (k=1)', polyleven.levenshtein, (1,))
