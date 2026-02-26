#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <libpmem.h>
#include <smmintrin.h>

#define PATH       "/dev/dax0.0"
#define ITERATIONS 256
#define OUTPUT     "results.txt"

static double timespec_diff_ns(struct timespec *start, struct timespec *end) {
    return (double)(end->tv_sec - start->tv_sec) * 1e9 +
           (double)(end->tv_nsec - start->tv_nsec);
}

int main(void) {
    int is_pmem;
    size_t mapped_len;

    char *pmem_addr = pmem_map_file(PATH, 0, 0, 0, &mapped_len, &is_pmem);
    if (pmem_addr == NULL) {
        perror("pmem_map_file");
        exit(1);
    }

    if (!is_pmem)
        fprintf(stderr, "Warning: not on a PMem-capable device\n");

    printf("Mapped %zu bytes, is_pmem=%d\n", mapped_len, is_pmem);

    const char *msg     = "Hello, persistent memory!";
    size_t      msg_len = strlen(msg) + 1;
    char        buf[256];

    double write_ns[ITERATIONS];
    double read_ns[ITERATIONS];

    struct timespec t0, t1;

    for (int i = 0; i < ITERATIONS; i++) {
        pmem_flush(pmem_addr, msg_len);
        _mm_sfence();

        clock_gettime(CLOCK_MONOTONIC, &t0);
        pmem_memcpy_persist(pmem_addr, msg, msg_len);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        write_ns[i] = timespec_diff_ns(&t0, &t1);

        pmem_flush(pmem_addr, msg_len);
        _mm_sfence();

        clock_gettime(CLOCK_MONOTONIC, &t0);
        memcpy(buf, pmem_addr, msg_len);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        read_ns[i] = timespec_diff_ns(&t0, &t1);

        (void)buf[0];
    }

    FILE *f = fopen(OUTPUT, "w");
    if (!f) {
        perror("fopen");
        pmem_unmap(pmem_addr, mapped_len);
        exit(1);
    }

    fprintf(f, "iteration,write_ns,read_ns\n");
    for (int i = 0; i < ITERATIONS; i++)
        fprintf(f, "%d,%.2f,%.2f\n", i, write_ns[i], read_ns[i]);

    double write_sum = 0, read_sum = 0;
    double write_min = write_ns[0], write_max = write_ns[0];
    double read_min  = read_ns[0],  read_max  = read_ns[0];

    for (int i = 0; i < ITERATIONS; i++) {
        write_sum += write_ns[i];
        read_sum  += read_ns[i];
        if (write_ns[i] < write_min) write_min = write_ns[i];
        if (write_ns[i] > write_max) write_max = write_ns[i];
        if (read_ns[i]  < read_min)  read_min  = read_ns[i];
        if (read_ns[i]  > read_max)  read_max  = read_ns[i];
    }

    fprintf(f, "\n");
    fprintf(f, "summary,min_ns,max_ns,avg_ns\n");
    fprintf(f, "write,%.2f,%.2f,%.2f\n",
            write_min, write_max, write_sum / ITERATIONS);
    fprintf(f, "read,%.2f,%.2f,%.2f\n",
            read_min,  read_max,  read_sum  / ITERATIONS);

    fclose(f);
    printf("Results written to %s\n", OUTPUT);

    pmem_unmap(pmem_addr, mapped_len);
    return 0;
}