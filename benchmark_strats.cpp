#include <fcntl.h>
#include <iostream>
#include <chrono>
#include <unistd.h>
#include <x86intrin.h>
#include <sys/mman.h>

#define DEV_PATH "/dev/dax0.0"
#define DEV_SIZE (2 * 1024 * 1024ul)

struct PMemData {
    alignas(64) uint64_t a;
    alignas(64) uint64_t b;
    alignas(64) uint64_t c;
    alignas(64) uint64_t d;
};

const int ITERATIONS = 10000000;
const int UPDATES_PER_TX = 10000;

void test_non_temporal(PMemData* data) {
    for (int i = 0; i < ITERATIONS; ++i) {
        for (int j = 0; j < UPDATES_PER_TX; ++j) {
            _mm_stream_si64((long long*)&data->a, i + j);
            _mm_stream_si64((long long*)&data->b, i + j + 1);
            _mm_stream_si64((long long*)&data->c, i + j + 2);
            _mm_stream_si64((long long*)&data->d, i + j + 3);
        }
        _mm_sfence();
    }
}

void test_batched(PMemData* data) {
    for (int i = 0; i < ITERATIONS; ++i) {
        for (int j = 0; j < UPDATES_PER_TX; ++j) {
            data->a = i + j;
            data->b = i + j + 1;
            data->c = i + j + 2;
            data->d = i + j + 3;
        }
        _mm_clwb(&data->a);
        _mm_clwb(&data->b);
        _mm_clwb(&data->c);
        _mm_clwb(&data->d);
        _mm_sfence();
    }
}

int main() {
    int fd = open(DEV_PATH, O_RDWR);
    if (fd == -1) {
        perror("Open");
        return 1;
    }
    char *base = (char *)mmap(NULL, DEV_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (base == MAP_FAILED) {
        perror("map");
        close(fd);
        return 1;
    }

    std::cout << "Running the benchmark with " << ITERATIONS << " iterations and " << UPDATES_PER_TX << " updates per txn" << std::endl;

    PMemData* my_pmem = (PMemData*) base;
    auto start = std::chrono::high_resolution_clock::now();
    test_non_temporal(my_pmem);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Non-Temporal Time: " 
              << std::chrono::duration<double>(end - start).count() << "s\n";

    start = std::chrono::high_resolution_clock::now();
    test_batched(my_pmem);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Batched Flush Time: " 
              << std::chrono::duration<double>(end - start).count() << "s\n";
    munmap(base, DEV_SIZE);
    close(fd);
    return 0;
}