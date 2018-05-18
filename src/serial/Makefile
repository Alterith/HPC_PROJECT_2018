CC=g++
CFLAGS=-std=c++11 -g -O0
INC_USER=src/inc

all: clean project

valgrind: valgrind_clean
	valgrind --leak-check=full \
	--show-leak-kinds=all \
	--track-origins=yes \
	--verbose \
	--log-file=valgrind-out.txt \
	./project.out

valgrind_clean:
	rm -f *.txt

project: 
	$(CC) $(CFLAGS) -I $(INC_USER) src/*.cpp -o $@.out

clean:
	rm -f *.out *.o

latex:
	$(MAKE) -C latex
	cp latex/Project.pdf ./report.pdf
