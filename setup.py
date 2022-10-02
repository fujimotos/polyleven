import setuptools

with open('README.rst') as fp:
    README = fp.read()

setuptools.setup(
    name='polyleven',
    version='0.8',
    author='Fujimoto Seiji',
    author_email='fujimoto@ceptord.net',
    license='MIT License',
    description='A fast C-implemented library for Levenshtein distance',
    long_description=README,
    long_description_content_type='text/x-rst',
    url='https://ceptord.net/',
    ext_modules=[
        setuptools.Extension('polyleven', sources=['polyleven.c'])
    ],
    project_urls={
        'Documentation': 'https://ceptord.net/',
        'GitHub Mirror': 'https://github.com/fujimotos/polyleven'
    },
    zip_safe=False,
    python_requires='>=3.4',
    keywords=['Levenshtein distance'],
    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'Operating System :: OS Independent',
        'Programming Language :: Python :: 3',
        'Programming Language :: C',
        'License :: OSI Approved :: MIT License'
    ]
)
