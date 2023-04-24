CC = gcc
CFLAGS = -Wall -g
RM = rm -f
SHAREDLIB = -shared
DLIBFLAGS = -ldl


all: cmp copy encode decode stshell

cmp: cmp.o
	$(CC) $(CFLAGS) -o $@ $^

copy: copy.o
	$(CC) $(CFLAGS) -o $@ $^

encode: encode.o libcodecA.so libcodecB.so
	$(CC) $(CFLAGS) $(DLIBFLAGS) -o $@ encode.o

decode: decode.o libcodecA.so libcodecB.so
	$(CC) $(CFLAGS) $(DLIBFLAGS) -o $@ decode.o

stshell: stshell.o
	$(CC) $(CFLAGS) -o $@ $^

libcodecA.so: codecA.o
	$(CC) $(CFLAGS) $(SHAREDLIB) -o $@ $^

libcodecB.so: codecB.o
	$(CC) $(CFLAGS) $(SHAREDLIB) -o $@ $^

codecA.o: codecA.c codecA.h
	$(CC) $(CFLAGS) -fPIC -c $^

codecB.o: codecB.c codecB.h
	$(CC) $(CFLAGS) -fPIC -c $^


stshell.o: stshell.c 
	$(CC) $(CFLAGS) -c $^

%.o: %.c
	$(CC) $(CFLAGS) -c $^

.PHONY: all clean

clean:
	$(RM)  *.o *.a *.so *.dll *.h.gch *.dylib cmp copy encode decode stshell