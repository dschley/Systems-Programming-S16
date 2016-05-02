COMPILER = gcc
CCFLAGS  = -Wall
all: test1 test2 test3 test4 test5

test1: test1.c mymalloc.o
	$(COMPILER) $(CCFLAGS) -o test1 test1.c mymalloc.o

test2: test2.c mymalloc.o
	$(COMPILER) $(CCFLAGS) -o test2 test2.c mymalloc.o

test3: test3.c mymalloc.o
	$(COMPILER) $(CCFLAGS) -o test3 test3.c mymalloc.o

test4: test4.c mymalloc.o
	$(COMPILER) $(CCFLAGS) -o test4 test4.c mymalloc.o

test5: test5.c mymalloc.o
	$(COMPILER) $(CCFLAGS) -o test5 test5.c mymalloc.o

mymalloc.o: mymalloc.c mymalloc.h
	$(COMPILER) $(CCFLAGS) -c mymalloc.c 
clean:
	rm -f test1 test2 test3 test4 test5 mymalloc.o
