polyleven
=========

Polyleven is a C-implented Python library that:

 * Can compute Levenshtein distance between two strings.

 * Performs computations with the efficiency comparable to the
   best currently available Python libraries.

 * Is distributed under a permissive license like MIT/X, and hence
   is easy to embed into other programs.

Installation
------------

Clone this repository and run setup.py:

    $ git clone https://github.com/fujimotos/polyleven
    $ cd polyleven
    $ sudo python3 setup.py install

As of v0.1, polyleven supports Python 3.4 or later.

How to use polyleven
--------------------

Here is an example to use the polyleven module:

```python
from polyleven import levenshtein

# Just call `levenshtein()` function to compute the edit
# distance between two strings.
dist = levenshtein('abcde', 'abcd')
print(dist)  # 1

# If you have no interest in computing distance more than
# a certain threshold `k`, you can speed things up by
# passing the value as the third argument.
#
# For example, you can archive 2x throughput by setting the
# threshold parameter to two.
dist = levenshtein('abcde', 'abc', 2)
print(dist)  # 2

# In this case, the function will return (k + 1) when the
# input strings are more than `k` distance away.
dist = levenshtein('abcde', '', 2)
print(dist)  # 3
```
