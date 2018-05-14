CC=nvcc
CFLAGS=-std=c++11 -rdc=true
INC_USER=src/inc

all: clean project

project: 
	$(CC) $(CFLAGS) -I $(INC_USER) src/*.c* -o $@.out

clean:
	rm -f *.out *.o

latex:
	$(MAKE) -C latex
	cp latex/Project.pdf ./report.pdf
