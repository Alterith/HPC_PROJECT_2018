CC=mpicxx
CFLAGS=-std=c++11 -O3 -march=native
INC_USER=src/inc

all: clean project_mpi

project_mpi: 
	$(CC) $(CFLAGS) -I $(INC_USER) src/*.cpp -o $@.out

clean:
	rm -f *.out *.o

latex:
	$(MAKE) -C latex
	cp latex/Project.pdf ./report.pdf
