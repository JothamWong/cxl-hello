// Experiment to measure cl and clopt performance
#include <emmintrin.h>
#include <fcntl.h>
#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "utils.h"
#include <stdint.h>
#include <xmmintrin.h>

void clfl(void* start, size_t dev_size) {
  uint64_t* s = (uint64_t*) start;
  int num_iterations = (dev_size / sizeof(uint64_t));
  for (int i = 0; i < num_iterations; i += 8) {
    s[i] = i;
    _mm_clflush(&s[i]);
  }
}

void clflopt(void* start, size_t dev_size) {
  uint64_t* s = (uint64_t*) start;
  int num_iterations = (dev_size / sizeof(uint64_t));
  for (int i = 0; i < num_iterations; i += 8) {
    s[i] = i;
    _mm_clflushopt(&s[i]);
  }
  _mm_sfence();
}

int main(int argc, char** argv) {
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

  double start_clfl_time = get_time();
  clfl(base, DEV_SIZE);
  double end_clfl_time = get_time();
  double clfl_time = end_clfl_time - start_clfl_time;

  double start_clflopt_time = get_time();
  clflopt(base, DEV_SIZE);
  double end_clflopt_time = get_time();
  double clflopt_time = end_clflopt_time - start_clflopt_time;

  printf("clfl   : Took %fs for %luMB\n", clfl_time, DEV_SIZE);
  printf("clflopt: Took %fs for %luMB\n", clflopt_time, DEV_SIZE);

  munmap(base, DEV_SIZE);
  close(fd);
  return 0;
}
