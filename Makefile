VERSION = 0.3
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

archive:
	git archive --prefix polyleven-${VERSION}/ ${VERSION} \
      | gzip -9 > polyleven-$(VERSION).tar.gz

clean:
	$(PYTHON) setup.py clean
	rm -r build
	rm -r dist

.PHONY: all install bench bench-long test sdist clean archive
