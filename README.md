polyleven
=========

polyleven is yet another libary to compute Levenshtein distance.

 * Requires Python 3.3 or later (Python 2.X is not supported)

Installation
------------

Clone this repository:

    $ git clone https://github.com/fujimotos/polyleven
    $ cd polyleven

Run setup.py:

    $ sudo python3 setup.py install

Usage
-----

Here is the basic usage:

    >>> from polyleven import levenshtein
    >>> levenshtein("house", "noise")
    2

Also you can pass a maximum distance to compute as the third argument:

    >>> levenshtein("house", "noise", 2)
    2
    >>> levenshtein("house", "note", 2)
    3

Benchmarks
----------

...
