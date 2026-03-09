#include <fcntl.h>
#include <immintrin.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#include <xmmintrin.h>

#define DEV_PATH "/dev/dax0.0"
// 128 GB
#define DEV_SIZE (8 * 1024 * 1024 * 1024ul)
#define MAP_SIZE (DEV_SIZE / 2)

void bandwidth_write(char* addr, size_t bs) {
  __m512i val = _mm512_set1_epi64(0xABCDEF1234567890l);
  for (size_t i = 0; i < bs; i += 64) {
    _mm512_stream_si512((__m512i*)&addr[i], val);
  }
  _mm_sfence();
}

void bandwidth_read(char* addr, size_t bs) {
  volatile __m512i target;
  for (size_t i = 0; i < bs; i += 64) {
    target = _mm512_stream_load_si512((__m512i*)&addr[i]);
  }
}

double get_time() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: %s <node> <mode> <bs(MB)>\n", argv[0]);
    exit(1);
  }

  int node = atoi(argv[1]);
  off_t offset;
  if (node == 0) {
    offset = 0;
  } else if (node == 1) {
    offset = 64 * 1024 * 1024 * 1024UL; // 64GB
  } else {
    printf("Node must be 0 or 1\n");
    exit(1);
  }

  char mode = argv[2][0];
  int dev_flags;
  int mmap_prot;
  int mmap_flags;
  if (mode == 'r') {
    dev_flags = O_RDWR;
    mmap_prot = PROT_READ;
    mmap_flags = MAP_SHARED;
  } else if (mode == 'w') {
    dev_flags = O_RDWR;
    mmap_prot = PROT_WRITE;
    mmap_flags = MAP_SHARED;
  } else {
    printf("Mode must be 'r' or 'w'\n");
    exit(1);
  }

  size_t bs = (size_t)atoi(argv[3]) * 1024 * 1024;
  
  int fd = open(DEV_PATH, dev_flags);
  if (fd == -1) {
    perror("open");
    return 1;
  }

  char *base = (char *)mmap(NULL, MAP_SIZE, mmap_prot, mmap_flags, fd, offset);
  if (base == MAP_FAILED) {
    perror("mmap");
    close(fd);
    return 1;
  }

  printf("Benchmarking: Node %d | Mode %c | Batch Size %zu MB\n", node, mode, bs / (1024 * 1024));
  double start = get_time();
  for (size_t current_off = 0; current_off < MAP_SIZE; current_off += bs) {
    if (mode == 'w') {
        bandwidth_write(base + current_off, bs);
    } else {
        bandwidth_read(base + current_off, bs);
    }
  }
  double end = get_time();
  double duration = end - start;

  double total_gb = (double)DEV_SIZE / (1024 * 1024 * 1024);
  double bw = total_gb / duration;

  printf("Processed %.2f GB in %.4f seconds.\n", total_gb, duration);
  printf("Result: %.2f GB/s\n", bw);

  munmap(base, MAP_SIZE);
  close(fd);
  return 0;
}
