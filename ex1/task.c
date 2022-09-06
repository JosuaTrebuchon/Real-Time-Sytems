#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "time_functions.h"
#include <sys/times.h>
#include <string.h>
#include <sched.h>

void task_a_sleep(double seconds)
{
    sleep(seconds);
}

void task_a_usleep(useconds_t useconds)
{
    usleep(useconds);
}

void task_a_nanosleep(unsigned int seconds)
{
    struct timespec t, res;
    t.tv_nsec = 0;
    t.tv_sec = seconds;
    nanosleep(&t, &res);
}

void task_a_clockgettime(unsigned int seconds)
{
    struct timespec t;
    t.tv_nsec = 0;
    t.tv_sec = seconds;
    busy_wait(t);
}

void task_a_times(unsigned int seconds)
{
    busy_wait_times(seconds);
}

void task_b_rdtsc()
{

    struct timespec begin, end, elapsed;
    unsigned long latency;
    int i;

      
    clock_gettime(CLOCK_MONOTONIC, &begin);

    for(i = 0; i < 10*1000*1000; i++ ){
        rdtsc();
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    elapsed = timespec_sub(end, begin);

    printf("%lds, %ldns\n", elapsed.tv_sec, elapsed.tv_nsec);
    latency = ((unsigned long) (elapsed.tv_sec * 1000000000)) + ((unsigned long) (elapsed.tv_nsec));
    printf("%ldns\n", (unsigned long) (latency / i));
}

void task_b_clock_gettime()
{

    struct timespec begin, end, elapsed, now;
    unsigned long latency;
    int i;

      
    clock_gettime(CLOCK_MONOTONIC, &begin);

    for(i = 0; i < 10*1000*1000; i++ ){
        clock_gettime(CLOCK_MONOTONIC, &now);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    elapsed = timespec_sub(end, begin);

    printf("%lds, %ldns\n", elapsed.tv_sec, elapsed.tv_nsec);
    latency = ((unsigned long) (elapsed.tv_sec * 1000000000)) + ((unsigned long) (elapsed.tv_nsec));
    printf("%ldns\n", (unsigned long) (latency / i));
}

void task_b_times()
{

    struct timespec begin, end, elapsed;
    unsigned long latency;
    int i;

    struct tms now;
      
    clock_gettime(CLOCK_MONOTONIC, &begin);

    for(i = 0; i < 10*1000; i++ ){
        times(&now);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    elapsed = timespec_sub(end, begin);

    printf("%lds, %ldns\n", elapsed.tv_sec, elapsed.tv_nsec);
    latency = ((unsigned long) (elapsed.tv_sec * 1000000000)) + ((unsigned long) (elapsed.tv_nsec));
    printf("%ldns\n", (unsigned long) (latency / i));
}

void task_b_histogram_rdtsc(int ns_max)
{
    int histogram[ns_max];
    int i;
    u_int64_t t1, t2;

    memset(histogram, 0, sizeof(int) * ns_max);
    for(i = 0; i < 10*1000*1000; i++ ){
        t1 = rdtsc();
        t2 = rdtsc();

        int ns = (t2 - t1) * 1000000000 / 54000000;

        if(ns >= 0 && ns < ns_max) {
            histogram[ns]++;
        }
    }

    for (int i = 0; i < ns_max; i++){
        printf("%d\n", histogram[i]);
    }

}

void task_b_histogram_clockgettime(int ns_max)
{
    int histogram[ns_max];
    int i;
    struct timespec t1, t2, temp;

    memset(histogram, 0, sizeof(int) * ns_max);
    for(i = 0; i < 10*1000*1000; i++ ){
        clock_gettime(CLOCK_MONOTONIC, &t1);
        clock_gettime(CLOCK_MONOTONIC, &t2);

        temp = timespec_sub(t2, t1);

        int ns = ((unsigned long) (temp.tv_sec * 1000000000)) + ((unsigned long) (temp.tv_nsec));

        if(ns >= 0 && ns < ns_max) {
            histogram[ns]++;
        }
    }

    for (int i = 0; i < ns_max; i++){
        printf("%d\n", histogram[i]);
    }

}

void task_b_histogram_times(int ns_max)
{
    int histogram[ns_max];
    int i;
    struct tms t1, t2;

    memset(histogram, 0, sizeof(int) * ns_max);
    for(i = 0; i < 10*1000; i++ ){
        times(&t1);
        times(&t2);

        int ns = (t2.tms_utime - t1.tms_utime) * 1000000000;

        if(ns >= 0 && ns < ns_max) {
            histogram[ns]++;
        }
    }

    for (int i = 0; i < ns_max; i++){
        printf("%d\n", histogram[i]);
    }

}

void task_c(int us_max)
{
    int histogram[us_max];
    int i;
    struct timespec t1, t2, temp;

    memset(histogram, 0, sizeof(int) * us_max);
    for(i = 0; i < 1*1000*1000; i++ ){
        clock_gettime(CLOCK_MONOTONIC, &t1);
        sched_yield();
        clock_gettime(CLOCK_MONOTONIC, &t2);

        temp = timespec_sub(t2, t1);

        unsigned long us = (unsigned long) (((unsigned long) (temp.tv_sec * 1000000000)) + ((unsigned long) (temp.tv_nsec)) / 1000);

        if(us >= 0 && us < us_max) {
            histogram[us]++;
        }
    }

    for (int i = 0; i < us_max; i++){
        printf("%d\n", histogram[i]);
    }

}

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        printf("No argument given !\n");
        return -1;
    }
    switch(atoi(argv[1]))
    {
        case 1:
            // real	0m3.005s
            // user	0m0.001s
            // sys	0m0.005s
            task_a_sleep(3);
            break;
        case 2:
            // real	0m3.003s
            // user	0m0.003s
            // sys	0m0.000s
            task_a_usleep(3000000);
            break;
        case 3:
            // real	0m3.004s
            // user	0m0.001s
            // sys	0m0.004s
            task_a_nanosleep(3);
            break;
        case 4:
            // real	0m3.004s
            // user	0m2.990s
            // sys	0m0.001s
            task_a_clockgettime(3);
            break;  
        case 5: // Use the most system time
            // real	0m3.168s
            // user	0m3.000s
            // sys	0m0.161s
            task_a_times(3);
            break;
        case 6:
            printf("%ld\n", rdtsc());
            task_b_rdtsc();
            break;
        case 7:
            task_b_clock_gettime();
            break;
        case 8:
            task_b_times();
            break;
        case 9:
            task_b_histogram_rdtsc(50);
            break;
        case 10:
            task_b_histogram_clockgettime(50);
            break;
        case 11:
            task_b_histogram_times(500);
            break;
        case 12:
            task_c(15);
            break;
    }

    return 0;
}