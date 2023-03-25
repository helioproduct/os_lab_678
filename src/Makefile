CC = gcc	
BIN = main node control *.o *.so *.out ping
LIBS = -lzmq

all: control node

rebuild: clean all

clean:
	rm -f $(BIN)

# compile only
zmq_tools:
	$(CC) -c $(LIBS) zmq_tools.c -o zmq_tools.o

node.o:
	$(CC) -c node.c -o node.o

string_utils:
	$(CC) -c string_utils.c -o string_utils.o

# compile and link
node: node.o zmq_tools string_utils
	$(CC) $(LIBS) node.o zmq_tools.o string_utils.o -o node

control: string_utils control.c zmq_tools
	$(CC) $(LIBS) control.c string_utils.o zmq_tools.o -o control

ping: ping.c zmq_tools
	$(CC) $(LIBS) ping.c zmq_tools.o -o ping
