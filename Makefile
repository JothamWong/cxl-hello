CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGETS = read_dax write_dax pmdk_flush benchmark write_i read_i

all: $(TARGETS)

write_dax: write_dax.c
	$(CC) $(CFLAGS) -o write_dax write_dax.c

read_dax: read_dax.c
	$(CC) $(CFLAGS) -o read_dax read_dax.c

pmdk_flush: pmdk_flush.c
	$(CC) $(CFLAGS) -o pmdk_flush pmdk_flush.c -lpmem

benchmark: benchmark_strats.cpp
	g++ -O3 -march=native benchmark_strats.cpp -o benchmark

write_i: write_interactive.c
	$(CC) write_interactive.c -o write_i

read_i: read_i.c
	$(CC) read_i.c -o read_i

clean:
	rm -f $(TARGETS)

.PHONY: all clean