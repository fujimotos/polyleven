polyleven
=========

Polyleven is a C-implented Python library that:

 * Can compute Levenshtein distance between two strings.

 * Has the efficiency comparable to the best currently available Python
   libraries.

 * Is freely distributed (see LICENSE) and hence is easy to embed into
   other programs.

How to install
--------------

Clone this repository and run setup.py:

    $ git clone https://github.com/fujimotos/polyleven
    $ cd polyleven
    $ python3 setup.py install

As of v0.1, polyleven supports Python 3.4 or later.

Usage
-----

Polyleven provides a function `levenshtein()`, which computes the edit
distance between a pair of strings.

```python
>>> from polyleven import levenshtein
>>> levenshtein('abcde', 'abcd')
1
```

If you only need to compute k-or-fewer differences, you can speed up
things a lot by passing k as the third argument:

```python
>>> dist = levenshtein('abcde', 'abc', 2)
2
>>> dist = levenshtein('abcde', 'xxxxx', 2)
3
```

As you can see, `levenshtein()` returns (k + 1) when the distance
exceeds the given threshold k.

Benchmark
---------

To evaluate edit-distance libraries found on PyPI and Github, I used
a large English dictionary with 99,717 words (which is retrieved from
Debian's `wamerican` package).

With randomly chosen 10 input words, each library was used to compute
the edit distance with every word in the dictionary. The execution time
was measured using the `time` module in the Python standard library.

The system used for this benchmark was:

* Intel Core i3-4010U (1.70GHz)
* Linux x86-64 (Debian Stretch)
* Python 3.5.3 / GCC 6.3.0

### Result

 Test target                    |  TIME[sec]  |  SPEED[calls/s]
------------------------------- | ----------- | ----------------
edlib.align                     |   4.351     |      227,937
editdistance.eval               |   1.893     |      523,802
jellyfish.levenshtein\_distance |   0.757     |    1,309,227
distance.levenshtein            |   0.601     |    1,651,095
polyleven.levenshtein           |   0.516     |    1,920,087
Levenshtein.distance            |   0.470     |    2,108,491

Also, by setting a maximum threshold to compute, polyleven becomes
1.6-2.1x faster than the fastest general functions:

 Test target                    |  TIME[sec]  |  SPEED[calls/s]
------------------------------- | ----------- | ----------------
polyleven.levenshtein (k=3)     |   0.290     |    3,415,349
polyleven.levenshtein (k=2)     |   0.244     |    4,063,028
polyleven.levenshtein (k=1)     |   0.228     |    4,349,852

The script and data set used for this benchmark is available in the
`test` directory.

### List of libraries in this benchmark

* edlib (1.2.1) https://github.com/Martinsos/edlib
* editdistance (0.4) https://github.com/aflc/editdistance
* jellyfish (0.5.6) https://github.com/jamesturk/jellyfish
* distance (0.1.3) https://github.com/doukremt/distance
* polyleven (0.1) https://github.com/fujimotos/polyleven
* Python-Levenshtein (0.12) https://github.com/ztane/python-Levenshtein/
