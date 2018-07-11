#!/usr/bin/python3

from itertools import product
import os.path

def wagner_fischer(s, t):
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


def main():
    chars = ('', 'a', 'b', 'c')
    patterns = sorted(set("".join(s) for s in product(*[chars]*4)))

    for w1 in patterns:
        for w2 in patterns:
            print(wagner_fischer(w1, w2), w1, w2, sep='|')

if __name__ == '__main__':
    main()

