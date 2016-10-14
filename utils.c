#define _GNU_SOURCE

#include <sched.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <assert.h>
#include <stdint.h>

#include "utils.h"

#define SLEEP_NSEC      1E6L  /* 1 millisecond = 10^6 nanoseconds */
#define THRESHOLD_NSEC 1000L  /* (100 ns = Main Memory Reference) x 10  */

static uint64_t start = 0;

void access_counter(unsigned *hi, unsigned *lo) {
    __asm__ volatile
        ( "rdtsc; movl %%edx, %0; movl %%eax, %1" /* format string */
        : "=r" (*hi), "=r" (*lo) /* output list */
        : /* no inputs */
        : "%edx", "%eax"); /* clobber list */
}

void start_counter() {
    unsigned hi, lo;
    access_counter(&hi, &lo);
    start = ((uint64_t)hi << 32) | lo;
}

uint64_t get_counter() {
    unsigned ncyc_hi, ncyc_lo;
    access_counter(&ncyc_hi, &ncyc_lo);
    return (((uint64_t)ncyc_hi << 32) | ncyc_lo) - start;
}

uint64_t inactive_periods(int num, uint64_t threshold, uint64_t *samples) {
    uint64_t start;
    uint64_t end;
    uint64_t cur;

    start_counter();

    start = get_counter();
    end = start;
    cur = start;
    int i = 0;
    while (i < num) {
        end = cur;
        cur = get_counter();

        if (cur - end >= threshold) {
            samples[2 * i] = end;
            samples[2 * i + 1] = cur;
            i++;
            
        }
        
    } 

    return start;
}

float get_ms(uint64_t cycles, uint64_t cpu_freq) {
	return (cycles / (cpu_freq*10e6)) * 10e3;
}

void pin_process (int cpu) {
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(cpu, &set);

	if (sched_setaffinity(getpid(), sizeof(cpu_set_t), &set) == -1) {
		perror("Process pinning error");
		exit(1);
	}
}

// for qsort 
int cmpfunc (const void * a, const void * b) {
	return (*(uint64_t*)a - *(uint64_t*)b);
}

double frequency_test() {

	int sleep_samples = 5;
	double min_diff = -1;
	int i;
	uint64_t freq_list[sleep_samples];

	/* Get frequency from different sleep times*/
	for (i = 2; i <= sleep_samples + 1; i++) {
		start_counter();

		sleep(i/2);
		freq_list[i-2] = (double) get_counter()/((i/2) * 1e6);
	}

	qsort(freq_list, sleep_samples, sizeof(uint64_t), cmpfunc);

	int j;
	uint64_t cpu_freq;

	// Using K-best measurements here for cpu frequency measurement.
	// 5 samples, but best of 2 , we take the highest frequency of the
	// closest 2 samples

	double diff;
	for (j = 1; j < sleep_samples - 1; j++) {
		
		diff = freq_list[j] - freq_list[j-1];
		if ( diff <= min_diff || min_diff == -1) {
			min_diff = diff;
			cpu_freq = freq_list[j];
		}
	}

	return cpu_freq;
}

void print_results(int num_periods, uint64_t start, uint64_t *samples, uint64_t cpu_freq) {
	int period_id = 0;
	int i;
	for (i = 0; i < 2 * num_periods; i = i + 2) {

		printf("Active %d: start at %ld, duration %ld cycles (%f ms)\n", 
			period_id, start, samples[i] - start, get_ms(samples[i] - start, cpu_freq));
		
		start = samples[i];
		
		printf("Inactive %d: start at %ld, duration %ld cycles (%f ms)\n", 
			period_id, start, samples[i + 1] - start, get_ms(samples[i + 1] - start, cpu_freq));
		
		start = samples[i + 1];
		period_id = period_id + 1;
	}
}

float test_uncached(uint64_t cpu_freq){
  
  long int CACHE_LINE_SIZE = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
  //  printf("%d\n", CACHE_LINE_SIZE);
  int n = CACHE_LINE_SIZE/4;
  int array[n][n];
  start_counter();
  int i, j;
  for (i = 0; i < n; i++){
    for (j = 0; j < n; j++){
      array[j][i] = array[j][i] * 2;
    }
  }

  float uncached = get_ms(get_counter(), cpu_freq);

  start_counter();
  for (j = 0; j < n; j++){
    for (i = 0; i < n; i++){
      array[j][i] = array[j][i] * 2;
    }
  }

  float cached= get_ms(get_counter(), cpu_freq);
  //printf("hello");
  float delta = uncached - cached;
  return delta;
}