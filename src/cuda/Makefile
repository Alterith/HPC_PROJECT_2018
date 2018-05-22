CC=nvcc
CFLAGS=-std=c++11 -rdc=true -D_MWAITXINTRIN_H_INCLUDED
INC_USER=src/inc
INC=inc

all: clean project

project: 
	$(CC) $(CFLAGS) -I $(INC) -I $(INC_USER) src/*.c* -o $@.out

clean:
	rm -f *.out *.o

latex:
	$(MAKE) -C latex
	cp latex/Project.pdf ./report.pdf
