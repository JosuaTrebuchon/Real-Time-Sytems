struct timespec timespec_normalized(time_t sec, long nsec);

struct timespec timespec_sub(struct timespec lhs, struct timespec rhs);

struct timespec timespec_add(struct timespec lhs, struct timespec rhs);

int timespec_cmp(struct timespec lhs, struct timespec rhs);

void busy_wait(struct timespec t);

void busy_wait_times(/*clock_t c*/);

u_int64_t rdtsc(void);