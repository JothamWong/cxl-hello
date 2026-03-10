CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -O2 -I. -march=native -g
CXXFLAGS = -O3 -I. -march=native
TARGETS = read_dax write_dax benchmark write_i read_i conc clexp

all: $(TARGETS)

write_dax: write_dax.c utils.h
	$(CC) $(CFLAGS) -o $@ $<

read_dax: read_dax.c utils.h
	$(CC) $(CFLAGS) -o $@ $<

pmdk_flush: pmdk_flush.c utils.h
	$(CC) $(CFLAGS) -o $@ $< -lpmem

benchmark: benchmark_strats.cpp utils.h
	$(CXX) $(CXXFLAGS) -o $@ $<

write_i: write_interactive.c utils.h
	$(CC) $(CFLAGS) -o $@ $<

read_i: read_i.c utils.h
	$(CC) $(CFLAGS) -o $@ $<

conc: conc.c utils.h
	$(CC) $(CFLAGS) -mavx512f -o $@ $<

clexp: clexp.c utils.h
	$(CC) $(CFLAGS) -mavx512f -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean