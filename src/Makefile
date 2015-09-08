# CS165 MakeFile
# By the end of the week, we will include automatic dependency tracking
# so that adding additional files/dependencies will be easy.

CC = gcc -std=c99 -g -ggdb3

O ?= 3
CFLAGS = -Wall -Wextra -pedantic -pthread -O$(O)
LDFLAGS =
LIBS =
INCLUDES = -Iinclude

# Default optimization level

all: client server

shared_headers = $(addprefix include/,utils.h message.h common.h)

utils.o: utils.c $(shared_headers)
	$(CC) $(CFLAGS) $(DEPCFLAGS) $(INCLUDES) -c -o $@ $<

client.o: client.c $(shared_headers)
	$(CC) $(CFLAGS) $(DEPCFLAGS) $(INCLUDES) -c -o $@ $<

server.o: server.c $(shared_headers) include/cs165_api.h
	$(CC) $(CFLAGS) $(DEPCFLAGS) $(INCLUDES) -c -o $@ $<

client: client.o utils.o
	$(CC) $(CFLAGS) $(DEPCFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

server: server.o utils.o
	$(CC) $(CFLAGS) $(DEPCFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

clean:
	rm -f client server *.o *~ *.bak core *.core cs165_unix_socket

distclean: clean
	rm -rf $(DEPSDIR)

.PHONY: all clean distclean 
