#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>

#define DEV_PATH "/dev/dax0.0"
#define DEV_SIZE (2 * 1024 * 1024ul)

int main() {
    int fd = open(DEV_PATH, O_RDONLY);
    if (fd == -1) {
        perror("Error opening device");
        return 1;
    }

    char *base = (char *)mmap(NULL, DEV_SIZE, PROT_READ, MAP_SHARED, fd, 0);
    if (base == MAP_FAILED) {
        perror("mmap failed");
        close(fd);
        return 1;
    }

    size_t *len_ptr = (size_t *)base;
    char *data_ptr = base + sizeof(size_t);
    size_t stored_len = *len_ptr;
    printf("--- Reading from DAX ---\n");
    printf("Metadata Length: %zu bytes\n", stored_len);
    if (stored_len > 0 && stored_len < (DEV_SIZE - sizeof(size_t))) {
        printf("Message Content: %.*s\n", (int)stored_len, data_ptr);
    } else {
        printf("No valid data or empty message found.\n");
    }

    munmap(base, DEV_SIZE);
    close(fd);
    return 0;
}