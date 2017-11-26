from distutils.core import setup, Extension

setup(
    name = 'polyleven',
    version = '1.0',
    ext_modules = [
        Extension('polyleven', sources=['polyleven.c'])
    ]
)
