CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra
TARGET := disk_scheduler
OBJS := dask.o

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

dask.o: dask.cpp dask.h
	$(CXX) $(CXXFLAGS) -c dask.cpp -o $@

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: clean