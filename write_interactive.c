#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <immintrin.h>
#include <stdint.h>

#define DEV_PATH "/dev/dax0.0"
#define DEV_SIZE (2 * 1024 * 1024ul) 
#define INPUT_SIZE 1024

int main() {
    int fd = open(DEV_PATH, O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("Error opening device");
        return 1;
    }

    char *base = (char *)mmap(NULL, DEV_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (base == MAP_FAILED) {
        perror("mmap failed");
        close(fd);
        return 1;
    }

    size_t *len_ptr = (size_t *)base;
    char *data_ptr = base + sizeof(size_t);

    char input_buffer[INPUT_SIZE];
    printf("Enter: ");
    if (fgets(input_buffer, INPUT_SIZE, stdin) != NULL) {
        input_buffer[strcspn(input_buffer, "\n")] = 0;
        size_t input_len = strlen(input_buffer);
        *len_ptr = input_len;
        strcpy(data_ptr, input_buffer);
        for (uintptr_t i = (uintptr_t)base; i < (uintptr_t)(data_ptr + input_len); i += 64) {
            _mm_clflush((void *)i);
        }
        _mm_sfence();
    }

    munmap(base, DEV_SIZE);
    close(fd);
    return 0;
}