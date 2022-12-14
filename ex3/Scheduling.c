// clang -lpthread -std=gnu11 main.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

#define timeUnitUs 100 * 1000 // about 100 ms

pthread_mutex_t taskStateMutex;

enum taskState
{
	idle,
	running,
	preempted,
	finished
};
const char *taskStateString[4] = {"idle", "running", "preempted", "finished"};

struct Task
{
	enum taskState state;
	int ID;
	int startTime;		// In some imaginary integer time unit
	int totalRuntime;	// In some imaginary integer time unit
	int currentRuntime; // In some imaginary integer time unit
};

void set_task_state(struct Task *task, enum taskState taskNewState)
{
	pthread_mutex_lock(&taskStateMutex);
	task->state = taskNewState;
	pthread_mutex_unlock(&taskStateMutex);
}

void *task_handler(void *var)
{
	struct Task *task;
	task = (struct Task *)var;
	enum taskState prevTaskState = task->state;
	printf("Task handler initiated for task with ID %d in state %s \n", task->ID, taskStateString[task->state]);
	fflush(stdout);
	while (task->currentRuntime < task->totalRuntime)
	{
		if (task->state != prevTaskState)
		{
			enum taskState taskOldState = prevTaskState;
			prevTaskState = task->state;
			printf("Task with ID %d changed state from %s to %s after having worked for %d time units \n", task->ID, taskStateString[taskOldState], taskStateString[task->state], task->currentRuntime);
			fflush(stdout);
		}
		if (task->state == running)
		{
			task->currentRuntime++;
		}
		usleep(timeUnitUs);
	}
	printf("Task with ID %d finising after having worked for %d out of %d time units \n", task->ID, task->currentRuntime, task->totalRuntime);
	fflush(stdout);
	set_task_state(task, finished);
	return NULL;
}

void wait_for_rescheduling(int tick_time_units, struct Task *task)
{
	int runtime = 0;
	while (task->state != finished && runtime < tick_time_units)
	{
		usleep(timeUnitUs);
		runtime++;
	}
}

void round_robin(struct Task **tasks, int taskCount, int timeout, int interval)
{
	printf("Started RR scheduler with %d tasks \n", taskCount);
	fflush(stdout);
	int totalTimePassed = 0;
	int currentTaskIndex = 0;
	int prevTaskIndex = -1;
	int counter = 0;
	while (1)
	{
		printf("Scheduler running at time %d \n", totalTimePassed);
		fflush(stdout);

		if (totalTimePassed >= timeout)
		{
			printf("Scheduler done \n");
			fflush(stdout);
			return;
		}

		counter = 0;
		while (counter < taskCount)
		{
			currentTaskIndex = (currentTaskIndex + 1) % taskCount;
			if (tasks[currentTaskIndex]->state != finished && tasks[currentTaskIndex]->startTime <= totalTimePassed)
			{
				break;
			}
			counter++;
		}

		if (counter >= taskCount)
		{
			totalTimePassed++; // Speeding through areas without tasks to schedule
		}
		else
		{
			if (prevTaskIndex != currentTaskIndex && prevTaskIndex != -1 && tasks[prevTaskIndex]->state != finished)
			{ // prevTaskIndex == -1 means that this is the first task to be selected
				set_task_state(tasks[prevTaskIndex], preempted);
			}
			set_task_state(tasks[currentTaskIndex], running);
			int previousTaskRuntime = tasks[currentTaskIndex]->currentRuntime;
			wait_for_rescheduling(interval, tasks[currentTaskIndex]);
			totalTimePassed += (tasks[currentTaskIndex]->currentRuntime - previousTaskRuntime);
			prevTaskIndex = currentTaskIndex;
		}
	}
}

void fifo_scheduling(struct Task **tasks, int taskCount, int timeout)
{
	printf("Started RR scheduler with %d tasks \n", taskCount);
	fflush(stdout);
	int totalTimePassed = 0;
	int currentTaskIndex = 0;
	// int prevTaskIndex = -1;
	int counter = 0;
	int counter_finished = 0;
	int lowestStartTime = 0; // tasks[0]->startTime;
	int ilowestStartTime = 0;
	while (1)
	{
		printf("Scheduler running at time %d \n", totalTimePassed);
		fflush(stdout);

		if (totalTimePassed >= timeout)
		{
			printf("Scheduler done \n");
			fflush(stdout);
			return;
		}

		// Find the lowest start time that still haven't been launched
		counter = 0;
		lowestStartTime = timeout;
		counter_finished = 0;
		while (counter < taskCount)
		{
			// printf("Tested ID -> %d\n", counter);
			if (tasks[counter]->state == finished)
			{
				counter_finished++;
			}
			if (tasks[counter]->state != finished && tasks[counter]->startTime < lowestStartTime)
			{

				// printf("Tested ID PASSED -> %d\n", counter);
				ilowestStartTime = counter;
				lowestStartTime = tasks[counter]->startTime;
			}
			else
			{
				counter++;
			}
		}
		printf("Next Task to be executed : %d\n", ilowestStartTime);
		currentTaskIndex = ilowestStartTime;
		if (tasks[currentTaskIndex]->startTime > totalTimePassed || counter_finished == taskCount)
		{
			totalTimePassed++; // Speeding through areas without tasks to schedule
		}
		else
		{
			set_task_state(tasks[currentTaskIndex], running);
			int previousTaskRuntime = tasks[currentTaskIndex]->currentRuntime;
			wait_for_rescheduling(tasks[currentTaskIndex]->totalRuntime, tasks[currentTaskIndex]);

			totalTimePassed += (tasks[currentTaskIndex]->currentRuntime - previousTaskRuntime);
		}
	}
}

void shortest_scheduling(struct Task **tasks, int taskCount, int timeout)
{
	printf("Started RR scheduler with %d tasks \n", taskCount);
	fflush(stdout);
	int totalTimePassed = 0;
	int currentTaskIndex = 0;
	// int prevTaskIndex = -1;
	int counter = 0;
	int counter_finished = 0;
	int shortestTime = 0; // tasks[0]->startTime;
	int ishortestTime = 0;
	while (1)
	{
		printf("Scheduler running at time %d \n", totalTimePassed);
		fflush(stdout);

		if (totalTimePassed >= timeout)
		{
			printf("Scheduler done \n");
			fflush(stdout);
			return;
		}

		// Find the lowest start time that still haven't been launched
		counter = 0;
		shortestTime = timeout;
		counter_finished = 0;
		while (counter < taskCount)
		{
			// printf("Tested ID -> %d\n", counter);
			if (tasks[counter]->state == finished)
			{
				counter_finished++;
			}
			if (tasks[counter]->state != finished && tasks[counter]->totalRuntime < shortestTime)
			{

				// printf("Tested ID PASSED -> %d\n", counter);
				ishortestTime = counter;
				shortestTime = tasks[counter]->totalRuntime;
			}
			else
			{
				counter++;
			}
		}
		// printf("Next Task to be executed : %d\n", ishortestTime);
		currentTaskIndex = ishortestTime;
		if (tasks[currentTaskIndex]->startTime > totalTimePassed || counter_finished == taskCount)
		{
			totalTimePassed++; // Speeding through areas without tasks to schedule
		}
		else
		{
			set_task_state(tasks[currentTaskIndex], running);
			int previousTaskRuntime = tasks[currentTaskIndex]->currentRuntime;
			wait_for_rescheduling(tasks[currentTaskIndex]->totalRuntime, tasks[currentTaskIndex]);

			totalTimePassed += (tasks[currentTaskIndex]->currentRuntime - previousTaskRuntime);
		}
	}
}

void hrrn_scheduling(struct Task **tasks, int taskCount, int timeout)
{
	printf("Started RR scheduler with %d tasks \n", taskCount);
	fflush(stdout);
	int totalTimePassed = 0;
	int currentTaskIndex = 0;
	// int prevTaskIndex = -1;
	int counter = 0;
	int counter_finished = 0;
	double highestResponseTime = 0; // tasks[0]->startTime;
	int ihighestResponseTime = 0;
	int waitTime = 0;
	double responseTime = 0;
	while (1)
	{
		printf("Scheduler running at time %d \n", totalTimePassed);
		fflush(stdout);

		if (totalTimePassed >= timeout)
		{
			printf("Scheduler done \n");
			fflush(stdout);
			return;
		}

		// Find the lowest start time that still haven't been launched
		counter = 0;
		highestResponseTime = 0;
		counter_finished = 0;
		while (counter < taskCount)
		{
			// printf("Tested ID -> %d\n", counter);
			if (tasks[counter]->state == finished)
			{
				counter_finished++;
			}

			waitTime = totalTimePassed - tasks[counter]->startTime;
			responseTime = (double)((double)waitTime + tasks[counter]->totalRuntime) / tasks[counter]->totalRuntime;

			if (tasks[counter]->state != finished && responseTime > highestResponseTime)
			{

				// printf("Tested ID PASSED -> %d\n", counter);
				ihighestResponseTime = counter;
				highestResponseTime = responseTime;
			}
			else
			{
				counter++;
			}
		}
		printf("Highest response time : %f\n", highestResponseTime);
		currentTaskIndex = ihighestResponseTime;
		if (tasks[currentTaskIndex]->startTime > totalTimePassed || counter_finished == taskCount)
		{
			totalTimePassed++; // Speeding through areas without tasks to schedule
		}
		else
		{
			set_task_state(tasks[currentTaskIndex], running);
			int previousTaskRuntime = tasks[currentTaskIndex]->currentRuntime;
			wait_for_rescheduling(tasks[currentTaskIndex]->totalRuntime, tasks[currentTaskIndex]);

			totalTimePassed += (tasks[currentTaskIndex]->currentRuntime - previousTaskRuntime);
		}
	}
}

void shortest_remaining_time(struct Task **tasks, int taskCount, int timeout, int interval)
{
	printf("Started shortest_remaining_time scheduler with %d tasks \n", taskCount);
	fflush(stdout);
	int totalTimePassed = 0;
	int currentTaskIndex = 0;
	int shortestTime = timeout;
	int ishortestTime = 0;
	int prevTaskIndex = -1;
	int counter_finished = 0;
	int counter = 0;
	while (1)
	{
		printf("Scheduler running at time %d \n", totalTimePassed);
		fflush(stdout);

		if (totalTimePassed >= timeout)
		{
			printf("Scheduler done \n");
			fflush(stdout);
			return;
		}

		// Find the lowest start time that still haven't been launched
		counter = 0;
		shortestTime = timeout;
		counter_finished = 0;
		while (counter < taskCount)
		{
			// printf("Tested ID -> %d\n", counter);
			if (tasks[counter]->state == finished)
			{
				counter_finished++;
			}
			if (tasks[counter]->state != finished && (tasks[counter]->totalRuntime - tasks[counter]->currentRuntime) < shortestTime && (tasks[counter]->startTime <= totalTimePassed))
			{

				// printf("Tested ID PASSED -> %d\n", counter);
				ishortestTime = counter;
				shortestTime = (tasks[counter]->totalRuntime - tasks[counter]->currentRuntime);
			}
			else
			{
				counter++;
			}
		}
		currentTaskIndex = ishortestTime;

		if (tasks[currentTaskIndex]->startTime > totalTimePassed || counter_finished == taskCount)
		{					   // counter >= taskCount
			totalTimePassed++; // Speeding through areas without tasks to schedule
		}
		else
		{
			if (prevTaskIndex != currentTaskIndex && prevTaskIndex != -1 && tasks[prevTaskIndex]->state != finished)
			{ // prevTaskIndex == -1 means that this is the first task to be selected
				set_task_state(tasks[prevTaskIndex], preempted);
			}
			set_task_state(tasks[currentTaskIndex], running);
			int previousTaskRuntime = tasks[currentTaskIndex]->currentRuntime;
			wait_for_rescheduling(interval, tasks[currentTaskIndex]);
			totalTimePassed += (tasks[currentTaskIndex]->currentRuntime - previousTaskRuntime);
			prevTaskIndex = currentTaskIndex;
		}
	}
}

void feedback(struct Task **tasks, int taskCount, int timeout, int interval, int **priority_queue)
{
	printf("Started shortest_remaining_time scheduler with %d tasks \n", taskCount);
	fflush(stdout);
	int totalTimePassed = 0;
	int currentTaskIndex = 0;
	int shortestTime = timeout;
	int ishortestTime = 0;
	int prevTaskIndex = -1;
	int counter_finished = 0;
	int finished = 0;

	int queue_indexes[7] = {0, 0, 0, 0, 0, 0, 0};
	int queue_counter = 0;

	while (1)
	{
		printf("Scheduler running at time %d \n", totalTimePassed);
		fflush(stdout);

		if (totalTimePassed >= timeout)
		{
			printf("Scheduler done \n");
			fflush(stdout);
			return;
		}
		// Loop to check if need to preempte or not

		// Filling the queue with new tasks
		// Dedicated to new tasks
		int new_task = 0;
		for (int i = 0; i < taskCount; i++)
		{
			if (tasks[i]->startTime <= totalTimePassed && tasks[i]->state == idle)
			{
				printf("new task 1%d\n", i);
				priority_queue[0][queue_indexes[0]] = tasks[i]->ID;
				queue_indexes[0]++;
				new_task = 1;
			}
		}
		for (int i = 0; i < 7; i++)
		{
			printf("%d  ", queue_indexes[i]);
		}
		printf("\n");
		// preempted and shift the queue
		if (prevTaskIndex >= 0 && finished == 0 && new_task == 1)
		{
			// printf("shift 1\n");
			set_task_state(tasks[prevTaskIndex], preempted);

			for (int i = 0; i < taskCount - 1; i++)
			{
				priority_queue[queue_counter][i] = priority_queue[queue_counter][i + 1];
			}

			for (int i = 0; i < 7; i++)
			{
				printf("%d  ", queue_indexes[i]);
			}
			printf("\n");
			priority_queue[queue_counter + 1][queue_indexes[queue_counter + 1]] = prevTaskIndex;
			// printf("shift 2.1\n");
			queue_indexes[queue_counter]--;
			// printf("shift 2.2\n");
			queue_indexes[queue_counter + 1]++;
			// printf("shift 3\n");
		}

		queue_counter = 0;
		// Find the first not empty queue
		while (queue_counter < 7)
		{
			if (queue_indexes[queue_counter] > 0) // The queue is not empty
			{
				break;
			}
			queue_counter++;
		}
		printf("shift 2 : %d   | ", queue_counter);
		currentTaskIndex = priority_queue[queue_counter][0];

		if (queue_counter >= 6)
		{					   // counter >= taskCount
			totalTimePassed++; // Speeding through areas without tasks to schedule
			finished = 1;
		}
		else
		{

			set_task_state(tasks[currentTaskIndex], running);
			int previousTaskRuntime = tasks[currentTaskIndex]->currentRuntime;
			wait_for_rescheduling(interval, tasks[currentTaskIndex]);
			totalTimePassed += (tasks[currentTaskIndex]->currentRuntime - previousTaskRuntime);
			prevTaskIndex = currentTaskIndex;
		}
	}
}

void feedback2(struct Task **tasks, int taskCount, int timeout, int interval)
{

	printf("Started  scheduler with %d tasks \n", taskCount);
	fflush(stdout);
	int totalTimePassed = 0;
	int currentTaskIndex = 0;
	int shortestTime = timeout;
	int ishortestTime = 0;
	int prevTaskIndex = -1;
	int counter_finished = 0;
	int runningTasks = 0;

	int priority_queue[7][5] = {
		{-1, -1, -1, -1, -1},
		{-1, -1, -1, -1, -1},
		{-1, -1, -1, -1, -1},
		{-1, -1, -1, -1, -1},
		{-1, -1, -1, -1, -1},
		{-1, -1, -1, -1, -1},
		{-1, -1, -1, -1, -1},
	};

	int queue_indexes[7] = {0, 0, 0, 0, 0, 0, 0};
	int rr_queue_indexes[7] = {0, 0, 0, 0, 0, 0, 0};
	int queue_counter = 0;

	while (1)
	{
		printf("Scheduler running at time %d \n", totalTimePassed);
		fflush(stdout);

		if (totalTimePassed >= timeout)
		{
			printf("Scheduler done \n");
			fflush(stdout);
			return;
		}

		// Delete finished tasks
		for (int i = 0; i < 7; i++)
		{
			for (int j = 0; j < queue_indexes[i]; j++)
			{
				if (priority_queue[i][j] != -1 && tasks[priority_queue[i][j]]->state == finished)
				{
					// priority_queue[i][j] = -1;
					for (int k = j; k < 4; k++)
					{
						priority_queue[i][k] = priority_queue[queue_counter][k + 1];
					}
					priority_queue[queue_counter][4] = -1;
					queue_indexes[i]--;
					runningTasks--;
				}
			}
		}

		// Add new tasks to the first queue
		for (int i = 0; i < taskCount; i++)
		{
			// If tasks can strt and task had ot been running from the beginning
			if (tasks[i]->startTime <= totalTimePassed && tasks[i]->state == idle)
			{
				printf("new task %d\n", i);
				priority_queue[0][queue_indexes[0]] = tasks[i]->ID;
				queue_indexes[0]++;
				runningTasks++;
			}
		}

		if (runningTasks > 1)
		{
			// move queue
			if (tasks[prevTaskIndex]->state != finished)
			{

				priority_queue[queue_counter + 1][queue_indexes[queue_counter + 1]] = prevTaskIndex;
				queue_indexes[queue_counter]--;
				queue_indexes[queue_counter + 1]++;

				// Shift priority queue to the left
				for (int i = 0; i < taskCount - 1; i++)
				{
					priority_queue[queue_counter][i] = priority_queue[queue_counter][i + 1];
				}
				priority_queue[queue_counter][taskCount - 1] = -1;
			}
		}

		queue_counter = 0;
		// Find the first not empty queue
		while (queue_counter < 7)
		{
			if (queue_indexes[queue_counter] > 0) // The queue is not empty
			{
				break;
			}
			queue_counter++;
		}

		int counter = 0;
		int itask = -1;
		while (counter < taskCount && queue_counter < 7)
		{
			itask = priority_queue[queue_counter][rr_queue_indexes[queue_counter]];
			if (itask >= 0)
			{
				if (tasks[itask]->state != finished)
				{
					currentTaskIndex = priority_queue[queue_counter][rr_queue_indexes[queue_counter]];
					break;
				}
			}
			rr_queue_indexes[queue_counter] = (rr_queue_indexes[queue_counter] + 1) % taskCount;
			counter++;
		}
		// printf("running tasks %d\n", runningTasks);

		if (runningTasks == 0)
		{
			totalTimePassed++;
		}
		else
		{
			if (prevTaskIndex != currentTaskIndex && prevTaskIndex != -1 && tasks[prevTaskIndex]->state != finished)
			{
				set_task_state(tasks[prevTaskIndex], preempted);

				for (int i = 0; i < 7; i++)
				{
					printf("%d  ", queue_indexes[i]);
				}
				printf("\n");

				// Print priority queue
				/*
				for (int i = 0; i < 7; i++)
				{
					for (int j = 0; j < 5; j++)
					{
						printf("%d  ", priority_queue[i][j]);
					}
					printf("\n");
				}
				*/
			}
			set_task_state(tasks[currentTaskIndex], running);
			int previousTaskRuntime = tasks[currentTaskIndex]->currentRuntime;
			wait_for_rescheduling(interval, tasks[currentTaskIndex]);
			totalTimePassed += (tasks[currentTaskIndex]->currentRuntime - previousTaskRuntime);
			prevTaskIndex = currentTaskIndex;
		}
	}
}
// Implement your schedulers here!tasks[currentTaskIndex]

int main()
{
	printf("Main started \n");
	fflush(stdout);
	int taskCount = 5;
	pthread_t threads[taskCount];

	// {state, ID, start-time, total runtime, current runtime}
	struct Task task0 = {idle, 0, 00, 30, 0};
	struct Task task1 = {idle, 1, 15, 60, 0};
	struct Task task2 = {idle, 2, 35, 40, 0};
	struct Task task3 = {idle, 3, 55, 50, 0};
	struct Task task4 = {idle, 4, 75, 20, 0};

	struct Task *tasks[] = {&task0, &task1, &task2, &task3, &task4};
	// int **priority_queue = (int **)malloc(7 * sizeof(int *));

	/*
	for (int i = 0; i < 7; i++)
	{
		priority_queue[i] = (int *)malloc(5 * sizeof(int) * 8);
		memset(priority_queue[i], -1, 5);
	}
	*/

	for (int i = 0; i < taskCount; i++)
	{
		pthread_create(&threads[i], NULL, task_handler, (void *)tasks[i]);
	}

	sleep(1); // Let everything stabilize
	int schedulerTimeout = 250;

	// round_robin(tasks, taskCount, schedulerTimeout, 10);
	// fifo_scheduling(tasks, taskCount, schedulerTimeout);
	// shortest_scheduling(tasks, taskCount, schedulerTimeout);
	// hrrn_scheduling(tasks, taskCount, schedulerTimeout);
	// shortest_remaining_time(tasks, taskCount, schedulerTimeout, (int) 10);
	feedback2(tasks, taskCount, schedulerTimeout, (int)10);

	for (int i = 0; i < taskCount; i++)
	{
		pthread_join(threads[i], NULL);
	}

	printf("Main done \n");
	fflush(stdout);
	/*
	for (int i = 0; i < 6; i++)
	{
		free(priority_queue[i]);
	}
	free(priority_queue);
	*/
	return 0;
}