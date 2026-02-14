# Variables
CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGETS = read_dax write_dax

# Default target to build both programs
all: $(TARGETS)

# Rule to build the write script
write_dax: write_dax.c
	$(CC) $(CFLAGS) -o write_dax write_dax.c

# Rule to build the read script
read_dax: read_dax.c
	$(CC) $(CFLAGS) -o read_dax read_dax.c

# Clean up binaries
clean:
	rm -f $(TARGETS)

.PHONY: all clean