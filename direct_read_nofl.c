#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <immintrin.h>

#define DEV_PATH "/dev/mem"
#define DEV_SIZE (2 * 1024 * 1024ul)
#define OFFSET (0x4080000000)

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s [offset]\n", argv[0]);
        return 1;  
    }
    unsigned long long dev_offset = strtoull(argv[1], NULL, 0);

    int fd = open(DEV_PATH, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    char *base = (char *)mmap(NULL, DEV_SIZE, PROT_READ, MAP_SHARED, fd, OFFSET + dev_offset);
    if (base == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    printf("Read from %s: ", DEV_PATH);
    for (int i = 0; i < 13; i++) {
        putchar(base[i]);
    }
    putchar('\n');

    munmap(base, DEV_SIZE);
    close(fd);
    return 0;
}
