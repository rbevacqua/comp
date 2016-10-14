void access_counter(unsigned *hi, unsigned *lo);

void start_counter();

uint64_t get_counter();

uint64_t inactive_periods(int num, uint64_t threshold, uint64_t *samples);

float get_ms(uint64_t cycles, uint64_t cpu_freq);

void pin_process (int cpu);

double frequency_test();

void print_results(int num_periods, uint64_t start, uint64_t *samples, uint64_t cpu_freq);

float test_uncached(uint64_t cpu_freq);