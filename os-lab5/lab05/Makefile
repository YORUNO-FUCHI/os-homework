CC = g++
CFLAGS = -Wall -g -std=c++11
TARGET = vmrp
OBJS = vmrp.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

vmrp.o: vmrp.cc vmrp.h
	$(CC) $(CFLAGS) -c vmrp.cc

clean:
	rm -f $(TARGET) $(OBJS)