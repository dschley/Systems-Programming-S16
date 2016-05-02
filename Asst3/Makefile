COMPILER = gcc
CCFLAGS  = -Wall
all: indexer.c indexer.h tkzr.o
	$(COMPILER) $(CCFLAGS) -o index indexer.c tkzr.o

tkzr.o: tkzr.c tkzr.h
	$(COMPILER) $(CCFLAGS) -c tkzr.c

clean:
	rm -f tkzr.o index
