polyleven
=========

Polyleven is a C-implemented Python library that can compute Levenshtein
distance between two strings. The focus of this library is efficiently.
To archive this goal, polyleven combines a number of special case algorithms
under the hood.

Requirements
------------

 - Python 3.4 or later
 - C compiler (e.g. GCC)

How to install
--------------

Install via pip:

    $ pip install git+https://github.com/fujimotos/polyleven

Or clone this repository and run setup.py:

    $ git clone https://github.com/fujimotos/polyleven
    $ cd polyleven
    $ python3 setup.py install

Usage
-----

Polyleven provides a function `levenshtein()` that computes the edit
distance between the input strings:

```python
>>> from polyleven import levenshtein
>>> levenshtein('abcde', 'abcd')
1
```

This function takes an (optional) third parameter, with which you can
specify the upper bound of distance to compute. This can reduce the
computational time significantly (see Benchmark below).

```python
>>> dist = levenshtein('abcde', 'abc', 2)
2
>>> dist = levenshtein('abcde', 'xxxxx', 2)
3
```

Benchmark
---------

To evaluate edit-distance libraries found on PyPI and GitHub, I used
an English dictionary with 99,717 words (which is retrieved from
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
edlib.align                     |  4.763      |      208,216
editdistance.eval               |  1.943      |      510,450
jellyfish.levenshtein\_distance |  0.722      |    1,374,081
distance.levenshtein            |  0.623      |    1,591,396
Levenshtein.distance            |  0.500      |    1,982,764
polyleven.levenshtein           |  0.431      |    2,303,420

Also, by setting a maximum threshold to compute, polyleven becomes
1.5-2.0x faster than the fastest general functions:

 Test target                    |  TIME[sec]  |  SPEED[calls/s]
------------------------------- | ----------- | ----------------
polyleven.levenshtein (k=3)     |  0.311      |    3,189,790
polyleven.levenshtein (k=2)     |  0.256      |    3,876,265
polyleven.levenshtein (k=1)     |  0.234      |    4,243,284

The script and data set used for this benchmark is available in the
`test` directory.

### The list of libraries

* edlib (1.2.1) https://github.com/Martinsos/edlib
* editdistance (0.4) https://github.com/aflc/editdistance
* jellyfish (0.5.6) https://github.com/jamesturk/jellyfish
* distance (0.1.3) https://github.com/doukremt/distance
* polyleven (0.3) https://github.com/fujimotos/polyleven
* Python-Levenshtein (0.12) https://github.com/ztane/python-Levenshtein/
