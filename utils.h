#ifndef UTILS_H
#define UTILS_H

#include <bits/types/struct_timeval.h>
#include <sys/time.h>
#include <unistd.h>

#define DEV_PATH "/dev/dax0.0"
#define DEV_SIZE (16 * 1024 * 1024ul)
#define CL_SIZE (64)


static inline double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double) tv.tv_sec + (double)tv.tv_usec / 1000000.0;
}

#endif