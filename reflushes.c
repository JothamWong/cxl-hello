/*
  Cache-line reflush microbenchmark

  Measure *flush latency* when repeatedly flushing the same cache line A,
  while varying the "reflush distance" (number of unique cache lines touched
  between two flushes of A). Example: A, B, C, D, A => distance = 3.

  The cycles spent issuing clflushopt+sfence for A, under different
  interleavings. This approximates allocator-induced “cache line reflush”
  behavior (re-flushing the same line shortly after a previous flush).

  Reference:
    PMAlloc: A Holistic Approach to Improving Persistent Memory Allocation

*/

#define _GNU_SOURCE
#include "utils.h"
#include <fcntl.h>
#include <inttypes.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

static inline void pin_to_cpu0(void) {
  cpu_set_t set;
  CPU_ZERO(&set);
  CPU_SET(0, &set);
  if (sched_setaffinity(0, sizeof(set), &set) != 0) {
    perror("sched_setaffinity");
  }
}

static inline uint64_t rdtsc_start(void) {
  unsigned aux;
  asm volatile("cpuid" : : "a"(0) : "rbx", "rcx", "rdx");
  return __builtin_ia32_rdtsc();
}

static inline uint64_t rdtsc_stop(void) {
  unsigned aux;
  uint64_t t = __builtin_ia32_rdtscp(&aux);
  asm volatile("cpuid" : : "a"(0) : "rbx", "rcx", "rdx");
  return t;
}

static inline void clflushopt_line(void *p) {
  asm volatile("clflushopt (%0)" : : "r"(p) : "memory");
}

static inline void sfence_full(void) {
  asm volatile("sfence" : : : "memory");
}

static inline void mfence_full(void) {
  asm volatile("mfence" : : : "memory");
}

static inline void clobber(void) {
  asm volatile("" : : : "memory");
}

/*
  Do one pattern:
    A, L1, L2, ... Ld, A
  where each Li is a distinct cache line (distance d).
  We time ONLY the second flush of A (the “reflush”), because that’s the one
  expected to stall if the previous flush is still “in flight”.
*/
static uint64_t measure_reflush_cycles(char *base, int distance, int iters) {
  volatile uint64_t sink = 0;
  char *A = base;
  // Warm-up and touch lines so they are mapped and avoid first-touch page faults
  for (int i = 0; i <= distance; i++) {
    volatile uint64_t *p = (volatile uint64_t *)(base + (size_t)i * CL_SIZE);
    sink += *p;
  }

  mfence_full();

  uint64_t total = 0;
  for (int k = 0; k < iters; k++) {
    *(volatile uint64_t *)A = (uint64_t)k;
    clobber();
    clflushopt_line(A);
    sfence_full();

    for (int i = 1; i <= distance; i++) {
      char *Li = base + (size_t)i * CL_SIZE;
      *(volatile uint64_t *)Li = (uint64_t)(k + i);
      clobber();
      clflushopt_line(Li);
      sfence_full();
    }

    *(volatile uint64_t *)A = (uint64_t)(k ^ 0xdeadbeefULL);
    clobber();

    uint64_t t0 = rdtsc_start();
    clflushopt_line(A);
    sfence_full();
    uint64_t t1 = rdtsc_stop();

    total += (t1 - t0);
    sink ^= *(volatile uint64_t *)A;
  }

  if (sink == 0x12345678ULL) fprintf(stderr, "sink=%" PRIu64 "\n", sink);

  return total / (uint64_t)iters;
}

int main(int argc, char **argv) {
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

  pin_to_cpu0();

  const int iters = 200000;

  printf("Cache-line reflush microbenchmark\n");
  printf("DEV_PATH=%s DEV_SIZE=%zu bytes iters=%d\n", DEV_PATH, DEV_SIZE, iters);

  for (int dist = 0; dist <= 16; dist++) {
    size_t needed = (size_t)(dist + 1) * CL_SIZE;
    if (needed > (size_t)DEV_SIZE) {
      fprintf(stderr, "DEV_SIZE too small for dist=%d\n", dist);
      break;
    }

    uint64_t cyc = measure_reflush_cycles(base, dist, iters);
    printf("distance=%2d  avg_reflush_cycles=%" PRIu64 "\n", dist, cyc);
  }

  munmap(base, DEV_SIZE);
  close(fd);
  return 0;
}