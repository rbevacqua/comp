#define _GNU_SOURCE

#include <stdint.h>
#include <sched.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <assert.h>

#include "utils.h"

void print_results_a2(int id, int num_periods, uint64_t start, uint64_t *samples, uint64_t cpu_freq) {
	int i;
	int period_id = 0;

	for (i = 0; i < 2 * num_periods; i = i + 2) {
		printf("Active %d: Child #: %d starts at %f ms, ends at %f ms (duration %f ms)\n", 
			period_id, id, get_ms(start, cpu_freq), get_ms(samples[i], cpu_freq), get_ms(samples[i] - start, cpu_freq));

		start = samples[i];

		printf("Inactive %d: Child #: %d starts at %f ms, ends at %f ms (duration %f ms)\n", 
			period_id, id, get_ms(start, cpu_freq), get_ms(samples[i+1], cpu_freq), get_ms(samples[i+1] - start, cpu_freq));

		start = samples[i+1];
		period_id = period_id + 1;
	}
}

int main(int argc, char *argv[]) {
	int num_processes = 2;
	int threshold = 2500;
	int num_inactives = 0;

	if (argc > 2) {
		printf("Usage: ./part_a2 (num_inactive) \n");
        return -1;
	}

	if (argc == 3) {
		num_processes = atoi(argv[2]);
	}

	num_inactives = atoi(argv[1]);
	

	pin_process(0);

	double cpu_freq = frequency_test();

	start_counter();

	uint64_t *samples = (uint64_t *)malloc(2 * num_inactives * sizeof(uint64_t));

	int i;
	for (i = 0; i < num_processes; i++) {
		pid_t pid = fork();

		if (pid == -1) {
			perror("fork failed\n");
			exit(1);

		} else if (pid == 0) {
			uint64_t start = inactive_periods(num_inactives, threshold, samples);

			print_results_a2(i, num_inactives, start, samples, cpu_freq);

			fflush(stdout);
			exit(0);
		}
	}

	// parent
	// checks that all forked processess ended cleanly
	for (i = 0; i < num_processes; i++) {
		int status;
		pid_t pid = wait(&status);

		if (pid == -1) {
			perror("Error occurred during wait\n");
			exit(1);
		} else if (!WIFEXITED(status)) {
			perror("child did not exit successfully\n");
			exit(1);
		}
	}

	free(samples);

	return 0;
}