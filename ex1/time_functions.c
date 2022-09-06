#include <time.h>
#include <sys/times.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct timespec timespec_normalized(time_t sec, long nsec){
    while(nsec >= 1000000000)
    {
        nsec -= 1000000000;
        ++sec;
    }
    while(nsec < 0){
        nsec += 1000000000;
        --sec;
    }
    return (struct timespec){sec, nsec};
}

struct timespec timespec_sub(struct timespec lhs, struct timespec rhs){
    return timespec_normalized(lhs.tv_sec - rhs.tv_sec, lhs.tv_nsec - rhs.tv_nsec);
}

struct timespec timespec_add(struct timespec lhs, struct timespec rhs){
    return timespec_normalized(lhs.tv_sec + rhs.tv_sec, lhs.tv_nsec + rhs.tv_nsec);
}
int timespec_cmp(struct timespec lhs, struct timespec rhs){
    if (lhs.tv_sec < rhs.tv_sec)
        return -1;
    if (lhs.tv_sec > rhs.tv_sec)
        return 1;
    return lhs.tv_nsec - rhs.tv_nsec;
}


void busy_wait(struct timespec t){
    struct timespec now;
    
    clock_gettime(CLOCK_MONOTONIC, &now);
    
    struct timespec then = timespec_add(now, t);
    
    while(timespec_cmp(now, then) < 0){
        for(int i = 0; i < 10000; i++){}
        clock_gettime(CLOCK_MONOTONIC, &now);
    }
}

void busy_wait_times(unsigned int seconds)
{
    struct tms now, then;
    // int tics_per_second;
    // tics_per_second = sysconf(_SC_CLK_TCK);

    times(&now);
    
    then.tms_utime = ((double) now.tms_utime)+ (100 * seconds);
    
    while(now.tms_utime < then.tms_utime)
    {
        for(int i = 0; i < 10000; i++){}
        times(&now);
    }

    // printf("tms_utime : %f\n", ((double) now.tms_utime)/tics_per_second);
    // printf("tms_stime : %f\n", ((double) now.tms_stime)/tics_per_second);
    // printf("tms_cutime : %f\n", ((double) now.tms_cutime)/tics_per_second);
    // printf("tms_cstime : %f\n", ((double) now.tms_cstime)/tics_per_second);
}

u_int64_t rdtsc(void) {
    u_int64_t val;
    asm volatile("isb; mrs %0, cntvct_el0; isb; " : "=r"(val) :: "memory");

    return val;
}