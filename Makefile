CC=g++
CFLAGS=-I. -lnuma -O0

OBJ = main.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

numa-latency: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

numa-latency.s: main.cpp
	$(CC) -S -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o *~ core numa-latency

run: numa-latency
	./numa-latency