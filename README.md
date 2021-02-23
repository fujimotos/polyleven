Polyleven
=========

Polyleven is a fast Levenshtein distance library for Python.

Project page: http://ceptord.net/20181215-polyleven.html

Install
-------

    $ pip install polyleven

Usage
-----

Polyleven provides a single interface function "levenshtein()". You
can use this function to measure the similarity of two strings.

    >>> from polyleven import levenshtein
    >>> levenshtein('aaa', 'ccc')
    3

If you only care about distances under a certain threshold, you can
pass the max threshold to the third argument.

    >>> levenshtein('acc', 'ccc', 1)
    1
    >>> levenshtein('aaa', 'ccc', 1)
    2

In general, you can gain a noticeable speed boost with threshold
k < 3.

Source code
-----------

    git clone http://ceptord.net/cgit/polyleven/

GitHub Mirror: https://github.com/fujimotos/polyleven

Send bug reports to fujimoto@ceptord.net.

License
-------

From v0.7, polyleven is released under MIT License. See LICENSE for
the full license text.
