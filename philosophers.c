// clang -lpthread -std=gnu11 UnboundedPriorityInversion.c
#define _GNU_SOURCE
#define CPU_ID 1

// includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <semaphore.h>
#include <inttypes.h>

// global variables
sem_t sem;
pthread_barrier_t barrier;
pthread_t p1, p2, p3, p4, p5;

pthread_mutex_t forks[5];
pthread_mutexattr_t attr;

void* philosopher(int* id);

int set_cpu(int cpu_number){
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);
	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void print_pri(pthread_t *thread, char *s){
	struct sched_param param;
	int policy;
	pthread_getschedparam(*thread, &policy, &param);
	printf("b:%i ", param.sched_priority);
	printf("%s \n",s);
}

void busy_wait_ms(int ms){
	uint64_t pval = 0;
	asm volatile("isb; mrs %0, cntvct_el0; isb; " : "=r"(pval) :: "memory");
	uint64_t val = pval;
	while(val < pval + ms * 54054){ // With no load the RPI actually runs at 54 MHz. Check with sudo dmesg | grep MHz
		asm volatile("isb; mrs %0, cntvct_el0; isb; " : "=r"(val) :: "memory");
	}
}

void create_and_start_task(pthread_t * thread, void * function, int policy, int priority, int* id){
	int ret = 0;
	pthread_attr_t tattr;
	struct sched_param param;
	ret += pthread_attr_init(&tattr);
	ret += pthread_attr_getschedparam(&tattr, &param);
	ret += pthread_attr_setschedpolicy(&tattr, policy);
	ret += pthread_attr_setinheritsched(&tattr, PTHREAD_EXPLICIT_SCHED);
	param.sched_priority = priority;
	ret += pthread_attr_setschedparam(&tattr, &param);
	ret += pthread_create(thread, &tattr, function, id);
	if(ret != 0) {
		printf("Error during task init!");
	}
}

int main(){
	// Setting up barrier and semaphore
	int threadNum = 5;
    sem_init(&sem, 0, 4);

	pthread_barrier_init(&barrier, NULL, threadNum);
	pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
    for(int i = 0; i < 5; i++)
    {
        pthread_mutex_init(&forks[i], &attr);
    }
	
	//sem_init(&sem, 0, 1);
    int id0 = 0;
    int id1 = 1;
    int id2 = 2;
    int id3 = 3;
    int id4 = 4;
	// Create tasks	
	int policy = SCHED_RR;
	create_and_start_task(&p1, philosopher, policy, 1, &id0);
	create_and_start_task(&p2, philosopher, policy, 1, &id1);
	create_and_start_task(&p3, philosopher, policy, 1, &id2);
	create_and_start_task(&p4, philosopher, policy, 1, &id3);
	create_and_start_task(&p5, philosopher, policy, 1, &id4);

    // Sleep for 100 ms while other tasks are started
	usleep(100*1000);

	printf("-------------------Start test-------------------\n");
	pthread_barrier_wait(&barrier);
	pthread_barrier_destroy(&barrier);
	
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
	pthread_join(p3, NULL);
	pthread_join(p4, NULL);
	pthread_join(p5, NULL);

	printf("-------------------End test-------------------\n");
	
	// Delete semaphore
	sem_destroy(&sem);
	
	printf("Finished\n");
	
	exit(EXIT_SUCCESS);
}


void* philosopher(int* id)
{
    set_cpu(CPU_ID);
	// printf("ID: %d\n",*id);
	pthread_barrier_wait(&barrier);

    printf("P%d starting\n", *id);
    int left = *id ;
    int right = (*id + 1) % 5;

    while(1)
    {        
        busy_wait_ms(100);

        sem_wait(&sem);
        pthread_mutex_lock(&forks[left]); // waiting for left fork to be available
        printf("P%d picked up its left fork: %d\n", *id, left);


        pthread_mutex_lock(&forks[right]);// waiting for right fork to be available
        printf("P%d picked up its right fork: %d\n", *id, right);

        printf("P%d Eating\n", *id);
        busy_wait_ms(100);

        pthread_mutex_unlock(&forks[left]); // release left fork 
        printf("P%d put down its left fork: %d\n", *id, left);

        pthread_mutex_unlock(&forks[right]);// release right fork
        printf("P%d put down its right fork: %d\n", *id, right);

        sem_post(&sem);


    }

    return NULL;
}
