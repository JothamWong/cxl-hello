CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGETS = read_dax write_dax pmdk_flush benchmark

all: $(TARGETS)

write_dax: write_dax.c
	$(CC) $(CFLAGS) -o write_dax write_dax.c

read_dax: read_dax.c
	$(CC) $(CFLAGS) -o read_dax read_dax.c

pmdk_flush: pmdk_flush.c
	$(CC) $(CFLAGS) -o pmdk_flush pmdk_flush.c -lpmem

benchmark: benchmark_strats.cpp
	g++ -O3 -march=native benchmark_strats.cpp -o benchmark

clean:
	rm -f $(TARGETS)

.PHONY: all clean