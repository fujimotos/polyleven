PYTHON = python3
INSTALL_OPTS = --user

all:

install:
	$(PYTHON) setup.py install $(INSTALL_OPTS) > /dev/null

bench: install
	$(PYTHON) test/benchmark.py

bench-long: install
	$(PYTHON) test/benchmark_long.py

test: install test/data/regress.txt
	$(PYTHON) test/regression.py

test/data/regress.txt:
	$(PYTHON) test/maketest.py > $@

sdist: clean
	$(PYTHON) setup.py sdist

clean:
	$(PYTHON) setup.py clean
	rm -r build
	rm -r dist

.PHONY: all install bench bench-long test sdist clean
