COMPILER = gcc
CCFLAGS  = -Wall
all: bankServer.c bankServer.h client.c client.h
	$(COMPILER) $(CCFLAGS) -pthread -o server bankServer.c
	$(COMPILER) $(CCFLAGS) -pthread -o client client.c

server: bankServer.c bankServer.h
	$(COMPILER) $(CCFLAGS) -pthread -o server bankServer.c

client: client.c client.h
	$(COMPILER) $(CCFLAGS) -pthread -o client client.c

clean:
	rm -f server client
