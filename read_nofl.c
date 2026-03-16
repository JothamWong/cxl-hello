#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
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
    
    int fd = open(DEV_PATH, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    unsigned long long dev_offset = strtoull(argv[1], NULL, 0);
    char *base = (char *)mmap(NULL, DEV_SIZE, PROT_READ, MAP_SHARED, fd, dev_offset);
    if (base == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }
    
    struct timespec start, end;
    volatile char sink;
    clock_gettime(CLOCK_MONOTONIC, &start);
    sink = base[0]; 
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
    printf("Read from %s: ", DEV_PATH);
    for (int i = 0; i < 13; i++) putchar(base[i]);
    printf("\nRead Latency: %.2f ns\n", elapsed);

    munmap(base, DEV_SIZE);
    close(fd);
    return 0;
}