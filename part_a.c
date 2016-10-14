#define _GNU_SOURCE

#include <stdint.h>
#include <sched.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "utils.h"

int main(int argc, char const *argv[]) {

    if (argc != 2) {
        printf("Usage: ./part_a (num_inactive)\n");
        return -1;
    }

    int num_inactives = atoi(argv[1]);

    if (num_inactives == 0) {
        printf("Usage: ./part_a (num_inactive)\n");
        return -1;
    }

    pin_process(0);
    
    double cpu_freq = frequency_test();

    uint64_t *samples = (uint64_t*)malloc(2 * num_inactives * sizeof(uint64_t));

    //uint64_t threshold = (uint64_t)(test_uncached(cpu_freq) * 10e3 * cpu_freq);

    uint64_t threshold = 2500;

    //printf("%f, %f\n", (double)threshold, cpu_freq);
    uint64_t start = inactive_periods(num_inactives, threshold, samples);

    print_results(num_inactives, start, samples, cpu_freq);

    free(samples);


    return 0;
}