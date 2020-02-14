import setuptools

setuptools.setup(
    name='polyleven',
    version='0.5',
    author='Fujimoto Seiji',
    author_email='fujimoto@ceptord.net',
    license='Public Domain',
    description='A fast C-implemented library for Levenshtein distance',
    long_description="""\
Polyleven is a C-implemented Python library that can compute Levenshtein
distance between two strings. The focus of this library is efficiently.
To archive this goal, polyleven combines a number of special case algorithms
under the hood.""",
    url='http://ceptord.net/20181215-polyleven.html',
    ext_modules=[
        setuptools.Extension('polyleven', sources=['polyleven.c'])
    ],
    zip_safe=False,
    python_requires='>=3.4',
    keywords=['Levenshtein distance'],
    classifiers=[
        'Development Status :: 4 - Beta',
        'Operating System :: OS Independent',
        'Programming Language :: Python :: 3',
        'Programming Language :: C',
        'License :: Public Domain',
    ]
)
