CC = gcc
CFLAGS = -g -Wall -std=c99 -fopenmp -mavx -mfma -pthread
LDFLAGS = -fopenmp
CUNIT = -lcunit
PYTHON = -I/usr/include/python3.12 -lpython3.12

install:
	pip install .

uninstall:
	pip uninstall -y numc

clean:
	rm -f *.o
	rm -f test
	rm -rf build
	rm -rf __pycache__

test:
	rm -f test
	$(CC) $(CFLAGS) mat_test.c matrix.c -o test $(LDFLAGS) $(CUNIT) $(PYTHON)
	./test

.PHONY: test