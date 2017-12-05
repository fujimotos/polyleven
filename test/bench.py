import time
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

    basedir = os.path.dirname(sys.argv[0])
    path = os.path.join(basedir, 'words_english.txt')

    with open(path) as fp:
        words = [x.strip() for x in fp]
    sample = random.sample(words, 10)

    def bench(name, func, args=None):
        return do_bench(words, sample, name, func, args)

    print('Words :', len(words))
    print('Sample:', len(sample))
    print('Total : %s calls\n' % (len(words) * len(sample)))

    print('%-30s %-15s %s' % ('#', 'TIME[sec]', 'SPEED[calls/s]'))

    bench('polyleven', polyleven.levenshtein)
    bench('polyleven (k=4)', polyleven.levenshtein, (4,))
    bench('polyleven (k=3)', polyleven.levenshtein, (3,))
    bench('polyleven (k=2)', polyleven.levenshtein, (2,))
    bench('polyleven (k=1)', polyleven.levenshtein, (1,))
