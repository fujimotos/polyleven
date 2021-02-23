import setuptools

with open('README.md') as fp:
    README = fp.read()

setuptools.setup(
    name='polyleven',
    version='0.7',
    author='Fujimoto Seiji',
    author_email='fujimoto@ceptord.net',
    license='MIT License',
    description='A fast C-implemented library for Levenshtein distance',
    long_description=README,
    long_description_content_type='text/markdown',
    url='http://ceptord.net/20181215-polyleven.html',
    ext_modules=[
        setuptools.Extension('polyleven', sources=['polyleven.c'])
    ],
    project_urls={
        'Documentation': 'http://ceptord.net/20181215-polyleven.html',
        'GitHub Mirror': 'https://github.com/fujimotos/polyleven'
    },
    zip_safe=False,
    python_requires='>=3.4',
    keywords=['Levenshtein distance'],
    classifiers=[
        'Development Status :: 4 - Beta',
        'Operating System :: OS Independent',
        'Programming Language :: Python :: 3',
        'Programming Language :: C',
        'License :: OSI Approved :: MIT License'
    ]
)
