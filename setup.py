import setuptools

setuptools.setup(
    ext_modules=[
        setuptools.Extension('polyleven', sources=['polyleven.c'])
    ]
)
