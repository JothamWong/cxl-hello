#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>

#define DEV_PATH "/dev/dax0.0"
#define DEV_SIZE (2 * 1024 * 1024ul)

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s [offset]\n", argv[0]);
        return 1;  
    }

    unsigned long long dev_offset = strtoull(argv[1], NULL, 0);
    int fd = open(DEV_PATH, O_RDWR);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    char *base = (char *)mmap(NULL, DEV_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, dev_offset);
    if (base == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    const char *msg = "HEHE THE TOOGA";
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);
    strcpy(base, msg);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
    printf("Write Latency: %.2f ns\n", elapsed);

    munmap(base, DEV_SIZE);
    close(fd);
    return 0;
}