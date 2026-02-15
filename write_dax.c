#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <immintrin.h>

#define DEV_PATH "/dev/dax0.0"
#define DEV_SIZE (2 * 1024 * 1024ul) // 2MB alignment

int main() {
    int fd = open(DEV_PATH, O_RDWR);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    char *base = (char *)mmap(NULL, DEV_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (base == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    const char *msg = "HELLO JOTHAM";
    strcpy(base, msg);

    _mm_clflush(base);
    _mm_sfence();

    munmap(base, DEV_SIZE);
    close(fd);
    return 0;
}