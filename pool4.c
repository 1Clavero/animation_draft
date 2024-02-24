#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define TOTAL_THREADS 12
#define TOTAL_TASKS 1000

typedef struct {
    void (*task_function)(void*);
    void* arg;
} Task;

typedef struct {
    pthread_t threads[TOTAL_THREADS];
    Task task_queue[TOTAL_TASKS];
    int task_count;
    int task_head;
    int task_tail;
} ThreadPool;

void* worker(void* arg) {
    ThreadPool* pool = (ThreadPool*)arg;
    while (1) {
        Task task;
        // Lock omitted since we're not using mutex
        if (pool->task_count > 0) {
            task = pool->task_queue[pool->task_head];
            pool->task_head = (pool->task_head + 1) % TOTAL_TASKS;
            pool->task_count--;

            // Unlock omitted since we're not using mutex
            task.task_function(task.arg);
        } else {
            // No tasks, sleep for a while
            usleep(1000);
        }
    }
    return NULL;
}

void submit_task(ThreadPool* pool, void (*task_function)(void*), void* arg) {
    // Lock omitted since we're not using mutex
    if (pool->task_count < TOTAL_TASKS) {
        pool->task_queue[pool->task_tail].task_function = task_function;
        pool->task_queue[pool->task_tail].arg = arg;
        pool->task_tail = (pool->task_tail + 1) % TOTAL_TASKS;
        pool->task_count++;
        // Unlock omitted since we're not using mutex
    } else {
        printf("Task queue is full!\n");
    }
}

void init_thread_pool(ThreadPool* pool) {
    pool->task_count = 0;
    pool->task_head = 0;
    pool->task_tail = 0;

    for (int i = 0; i < TOTAL_THREADS; ++i) {
        pthread_create(&pool->threads[i], NULL, worker, (void*)pool);
    }
}

void destroy_thread_pool(ThreadPool* pool) {
    // Cancel threads and wait for them to exit
    for (int i = 0; i < TOTAL_THREADS; ++i) {
        pthread_cancel(pool->threads[i]);
        pthread_join(pool->threads[i], NULL);
    }
}

void example_task(void* arg) {
    int* num = (int*)arg;
    printf("Task: %d\n", *num);
    usleep(500000);
}

int main() {
    ThreadPool pool;
    init_thread_pool(&pool);

    // Submit tasks
    for (int i = 0; i < 100; ++i) {
        int* num = malloc(sizeof(int));
        *num = i;
        submit_task(&pool, example_task, (void*)num);
    }

    // Wait for tasks to finish
    while (pool.task_count > 0) {
        usleep(1000);
    }

    destroy_thread_pool(&pool);

    return 0;
}
