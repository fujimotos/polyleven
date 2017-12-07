polyleven
=========

Polyleven is a C-implented Python library that:

 * Can compute Levenshtein distance between two strings through
   a sensible interface.

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

Usage
-----

Here is the basic usage:

```python
>>> from polyleven import levenshtein
>>> levenshtein('house', 'noise')
2
```

You can pass a distance to compute as the third argument:

```python
>>> levenshtein('house', 'noise', 2)
2
>>> levenshtein('house', 'note', 2)
3
```
