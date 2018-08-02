polyleven
=========

Polyleven is an efficient Levenshtein distance library for Python.

Installation
------------

    $ pip install polyleven

Note: You need Python >= 3.4 and a C compiler to install polyleven.

Usage
-----

Use `polyleven.levenshtein()` to compute Levenshtein distance:

```python
>>> from polyleven import levenshtein
>>> levenshtein('abcde', 'abcd')
1
```

You can pass a third argument, which denotes the maximum distance to
compute. This can make computations a lot faster.

```python
>>> dist = levenshtein('abcde', 'abc', 2)
2
>>> dist = levenshtein('abcde', 'xxxxx', 2)
3
```

Benchmark
---------

For this benchmark, I choose a random word and computed the Levenshtein
distance with every entry in a modest-sized English dictionary (100k
words). I repeated the procedure 10 times, using a different input word
each time.

The system used for this benchmark was:

* Intel Core i3-4010U (1.70GHz)
* Linux x86-64 (Debian Stretch)
* Python 3.5.3 / GCC 6.3.0

**Result**

 Test target                    |  TIME[sec]  |  SPEED[calls/s]
------------------------------- | ----------- | ----------------
edlib.align                     |  4.763      |      208,216
editdistance.eval               |  1.943      |      510,450
jellyfish.levenshtein\_distance |  0.722      |    1,374,081
distance.levenshtein            |  0.623      |    1,591,396
Levenshtein.distance            |  0.500      |    1,982,764
polyleven.levenshtein           |  0.431      |    2,303,420

**Result (with maximum threshold)**

 Test target                    |  TIME[sec]  |  SPEED[calls/s]
------------------------------- | ----------- | ----------------
polyleven.levenshtein (k=3)     |  0.311      |    3,189,790
polyleven.levenshtein (k=2)     |  0.256      |    3,876,265
polyleven.levenshtein (k=1)     |  0.234      |    4,243,284

Benchmark: How about longer inputs?
-----------------------------------

This test shows how each library behaves as the size of input strings
increases.

For this test, I generated 5000 pairs of strings for each size 16, 32,
64, 128, 256, 512, 1024. I measured the time each library takes to
process all the given pairs.

**Result**

 Test target                    | N=16  | N=32  | N=64  | N=128 | N=256 | N=512 | N=1024
------------------------------- | ----- | ----- | ----- | ----- | ----- | ----- | ------
edlib.align                     | 0.040 | 0.063 | 0.094 | 0.205 | 0.432 | 0.908 |  2.089
editdistance.eval               | 0.027 | 0.049 | 0.086 | 0.178 | 0.336 | 0.740 | 58.139
jellyfish.levenshtein\_distance | 0.009 | 0.032 | 0.118 | 0.470 | 1.874 | 8.877 | 42.848
distance.levenshtein            | 0.007 | 0.029 | 0.109 | 0.431 | 1.726 | 6.950 | 27.998
Levenshtein.distance            | 0.006 | 0.022 | 0.085 | 0.336 | 1.328 | 5.286 | 21.097
polyleven.levenshtein           | 0.003 | 0.005 | 0.010 | 0.043 | 0.149 | 0.550 |  2.109

List of libraries
-----------------

I used the following libraries in the benchmarks above:

* edlib (1.2.1) https://github.com/Martinsos/edlib
* editdistance (0.4) https://github.com/aflc/editdistance
* jellyfish (0.5.6) https://github.com/jamesturk/jellyfish
* distance (0.1.3) https://github.com/doukremt/distance
* polyleven (0.3) https://github.com/fujimotos/polyleven
* Python-Levenshtein (0.12) https://github.com/ztane/python-Levenshtein/

Links
-----

These articles explain the ideas implemented in polyleven:

- [Can we optimize the Wagner-Fischer algorithm?](https://ceptord.net/wagner-fischer/)
- [mbleven — A fast algorithm for k-bounded Levenshtein distance](https://ceptord.net/fastcomp/)
