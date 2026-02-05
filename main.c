#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define DEV_PATH "/dev/dax0.0"
// The alignment of the device is 2MB
#define DEV_SIZE (2 * 1024 * 1024ul)

int main() {
  size_t offset = 0;
  int fd = open(DEV_PATH, O_RDWR);
  if (fd == -1) {
    perror("open");
    abort();
  }
  char *base = (char *)mmap(NULL, DEV_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                            fd, offset);
  if (base == MAP_FAILED) {
    perror("mmap");
    abort();
  }
  const char *msg = "Hello\n";
  strcpy(base, msg);
  msync(base, strlen(msg) + 1, MS_SYNC);
  printf("%s", base);
  munmap(base, DEV_SIZE);
  close(fd);
}