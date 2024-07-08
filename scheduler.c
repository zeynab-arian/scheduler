#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#define MAX_TASKS 100
#define NUM_PROCESSORS 3

typedef struct {
    char name[10];
    int period;
    int execution_time;
    int resource[3];
    int processor;
    int repetitions;
    int deadlines[MAX_TASKS];
} Task;

typedef struct {
    int id;
    int utilization;
    Task* ready_queue[MAX_TASKS];
    int ready_queue_size;
    Task* running_task;
    pthread_t thread;
    pthread_mutex_t lock;
} Processor;

typedef struct {
    int R1;
    int R2;
    int R3;
} Resources;

Processor processors[NUM_PROCESSORS];
Resources resources;
Task tasks[MAX_TASKS];
int task_count = 0;

pthread_mutex_t resource_lock;
pthread_mutex_t waiting_queue_lock;

Task* waiting_queue[MAX_TASKS];
int waiting_queue_size = 0;

pthread_t main_thread;

void init_resources(int R1, int R2, int R3) {
    resources.R1 = R1;
    resources.R2 = R2;
    resources.R3 = R3;
}

bool allocate_resources(Task* task) {
    pthread_mutex_lock(&resource_lock);
    if (resources.R1 >= task->resource[0] && resources.R2 >= task->resource[1] && resources.R3 >= task->resource[2]) {
        resources.R1 -= task->resource[0];
        resources.R2 -= task->resource[1];
        resources.R3 -= task->resource[2];
        pthread_mutex_unlock(&resource_lock);
        return true;
    }
    pthread_mutex_unlock(&resource_lock);
    return false;
}

void release_resources(Task* task) {
    pthread_mutex_lock(&resource_lock);
    resources.R1 += task->resource[0];
    resources.R2 += task->resource[1];
    resources.R3 += task->resource[2];
    pthread_mutex_unlock(&resource_lock);
}

void add_to_waiting_queue(Task* task) {
    pthread_mutex_lock(&waiting_queue_lock);
    waiting_queue[waiting_queue_size++] = task;
    pthread_mutex_unlock(&waiting_queue_lock);
}

Task* get_from_waiting_queue() {
    pthread_mutex_lock(&waiting_queue_lock);
    if (waiting_queue_size == 0) {
        pthread_mutex_unlock(&waiting_queue_lock);
        return NULL;
    }
    Task* task = waiting_queue[0];
    for (int i = 1; i < waiting_queue_size; i++) {
        waiting_queue[i - 1] = waiting_queue[i];
    }
    waiting_queue_size--;
    pthread_mutex_unlock(&waiting_queue_lock);
    return task;
}

void* processor_thread(void* arg) {
    Processor* processor = (Processor*)arg;
    while (1) {
        pthread_mutex_lock(&processor->lock);
        if (processor->ready_queue_size > 0) {
            processor->running_task = processor->ready_queue[0];
            for (int i = 1; i < processor->ready_queue_size; i++) {
                processor->ready_queue[i - 1] = processor->ready_queue[i];
            }
            processor->ready_queue_size--;
            pthread_mutex_unlock(&processor->lock);

            sleep(processor->running_task->execution_time);

            release_resources(processor->running_task);
            processor->running_task->repetitions--;
            if (processor->running_task->repetitions > 0) {
                add_to_waiting_queue(processor->running_task);
            }
        } else {
            pthread_mutex_unlock(&processor->lock);
            Task* task = get_from_waiting_queue();
            if (task != NULL && allocate_resources(task)) {
                pthread_mutex_lock(&processor->lock);
                processor->ready_queue[processor->ready_queue_size++] = task;
                pthread_mutex_unlock(&processor->lock);
            }
        }
    }
    return NULL;
}

void read_input() {
    int R1, R2, R3;
    scanf("R1: %d R2: %d R3: %d", &R1, &R2, &R3);
    init_resources(R1, R2, R3);

    while (task_count < MAX_TASKS && scanf("%s %d %d %d %d %d %d %d", tasks[task_count].name, &tasks[task_count].period, &tasks[task_count].execution_time,
                 &tasks[task_count].resource[0], &tasks[task_count].resource[1], &tasks[task_count].resource[2], 
                 &tasks[task_count].processor, &tasks[task_count].repetitions) != EOF) {
        for (int i = 0; i < tasks[task_count].repetitions; i++) {
            tasks[task_count].deadlines[i] = tasks[task_count].period * (i + 1);
        }
        task_count++;
    }
}

void print_deadlines() {
    for (int i = 0; i < task_count; i++) {
        printf("%s Deadlines: ", tasks[i].name);
        for (int j = 0; j < tasks[i].repetitions; j++) {
            printf("%d", tasks[i].deadlines[j]);
            if (j < tasks[i].repetitions - 1) {
                printf(", ");
            }
        }
        printf("\n");
    }
}

void init_processors() {
    for (int i = 0; i < NUM_PROCESSORS; i++) {
        processors[i].id = i;
        processors[i].utilization = 0;
        processors[i].ready_queue_size = 0;
        processors[i].running_task = NULL;
        pthread_mutex_init(&processors[i].lock, NULL);
        pthread_create(&processors[i].thread, NULL, processor_thread, &processors[i]);
    }
}

void* main_thread_function(void* arg) {
    while (1) {
        printf("Resources: R1=%d R2=%d R3=%d\n", resources.R1, resources.R2, resources.R3);
        pthread_mutex_lock(&waiting_queue_lock);
        printf("Waiting Queue: ");
        for (int i = 0; i < waiting_queue_size; i++) {
            printf("%s ", waiting_queue[i]->name);
        }
        printf("\n");
        pthread_mutex_unlock(&waiting_queue_lock);

        for (int i = 0; i < NUM_PROCESSORS; i++) {
            pthread_mutex_lock(&processors[i].lock);
            printf("Processor %d:\n", processors[i].id);
            printf("  Utilization: %d%%\n", processors[i].utilization);
            printf("  Ready Queue: ");
            for (int j = 0; j < processors[i].ready_queue_size; j++) {
                printf("%s(%d) ", processors[i].ready_queue[j]->name, processors[i].ready_queue[j]->repetitions);
            }
            printf("\n  Running Task: %s\n", processors[i].running_task ? processors[i].running_task->name : "None");
            pthread_mutex_unlock(&processors[i].lock);
        }

        sleep(1);
    }
    return NULL;
}

void start_main_thread() {
    pthread_create(&main_thread, NULL, main_thread_function, NULL);
}

int main() {
    pthread_mutex_init(&resource_lock, NULL);
    pthread_mutex_init(&waiting_queue_lock, NULL);
    
    read_input();
    print_deadlines();
    init_processors();
    start_main_thread();

    pthread_join(main_thread, NULL);
    for (int i = 0; i < NUM_PROCESSORS; i++) {
        pthread_join(processors[i].thread, NULL);
    }

    return 0;
}
