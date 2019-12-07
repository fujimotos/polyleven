Polyleven
=========

Polyleven is a hyper fast Levenshtein distance library for Python.

Project page: http://ceptord.net/20181215-polyleven.html

Install
-------

Just run setup.py.

    $ python3 setup.py install

PyPI package is available too.

    $ pip install polyleven

Usage
-----

Use `levenshtein()` to compute Levenshtein distance.

    >>> from polyleven import levenshtein
    >>> levenshtein('abcde', 'abcd')
    1

You can pass an integer to the third argument, which limits the maximum
distance to compute. This makes the computation a lot more efficient.

    >>> dist = levenshtein('abcde', 'abc', 2)
    2
    >>> dist = levenshtein('abcde', 'xxxxx', 2)
    3

Source code
-----------

    git clone http://ceptord.net/cgit/polyleven/

GitHub Mirror: https://github.com/fujimotos/polyleven

Send bug reports to fujimoto@ceptord.net.

License
-------

Public Domain
