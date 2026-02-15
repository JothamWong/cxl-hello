#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <immintrin.h>

#define DEV_PATH "/dev/dax0.0"
#define DEV_SIZE (2 * 1024 * 1024ul)

int main() {
    int fd = open(DEV_PATH, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    char *base = (char *)mmap(NULL, DEV_SIZE, PROT_READ, MAP_SHARED, fd, 0);
    if (base == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }
    _mm_clflush(base);
    _mm_mfence();

    printf("Read from %s: ", DEV_PATH);
    for (int i = 0; i < 13; i++) {
        putchar(base[i]);
    }
    putchar('\n');

    munmap(base, DEV_SIZE);
    close(fd);
    return 0;
}