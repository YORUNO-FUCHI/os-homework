CC = gcc
CFLAGS = -g -Wall
HDRS = ipc.h smoker.h

all: supplier smoker

supplier: supplier.o ipc.o
	$(CC) $(CFLAGS) -o $@ $^

smoker: smoker.o ipc.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f supplier smoker *.o