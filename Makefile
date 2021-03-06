PYTHON = python3
INSTALL_OPTS = --user

all:

install:
	$(PYTHON) setup.py install $(INSTALL_OPTS) > /dev/null

bench:
	$(PYTHON) test/benchmark.py

bench-long:
	$(PYTHON) test/benchmark_long.py

test:
	$(PYTHON) test/regression.py

sdist: clean
	$(PYTHON) setup.py sdist

clean:
	$(PYTHON) setup.py clean
	rm -rf build
	rm -rf dist
	rm -rf polyleven.egg-info

.PHONY: all install bench bench-long test sdist clean
