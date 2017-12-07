from distutils.core import setup, Extension

setup(
    name = 'polyleven',
    version = '0.1',
    ext_modules = [
        Extension('polyleven', sources=['polyleven.c'])
    ]
)
