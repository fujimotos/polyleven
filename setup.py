import setuptools

setuptools.setup(
    name='polyleven',
    version='0.2',
    author='Fujimoto Seiji',
    author_email='fujimoto@ceptord.net',
    description='A fast C-implemented library for Levenshtein distance',
    url='https://github.com/fujimotos/polyleven',
    ext_modules=[
        setuptools.Extension('polyleven', sources=['polyleven.c'])
    ],
    zip_safe=False,
    python_requires='>=3.4',
    classifiers=(
        'Development Status :: 4 - Beta',
        'Operating System :: OS Independent',
        'Programming Language :: Python :: 3',
        'Programming Language :: C',
        'License :: Public Domain',
    )
)
