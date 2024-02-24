#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define TOTAL_THREADS 12
#define TOTAL_TASKS 100

typedef struct {
    void (*task_function)(int);
    int arg;
} Task;

typedef struct {
    pthread_t threads[TOTAL_THREADS];
    Task task_queue[TOTAL_TASKS];
    int task_count;
    int task_head;
    int task_tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} ThreadPool;

void* worker(void* arg) {
    ThreadPool* pool = (ThreadPool*)arg;
    while (1) {
        Task task;
        pthread_mutex_lock(&pool->mutex);
        while (pool->task_count == 0) {
            pthread_cond_wait(&pool->cond, &pool->mutex);
        }
        task = pool->task_queue[pool->task_head];
        pool->task_head = (pool->task_head + 1) % TOTAL_TASKS;
        pool->task_count--;
        pthread_mutex_unlock(&pool->mutex);

        task.task_function(task.arg);
    }
    return NULL;
}

void submit_task(ThreadPool* pool, void (*task_function)(int), int arg) {
    pthread_mutex_lock(&pool->mutex);
    if (pool->task_count < TOTAL_TASKS) {
        pool->task_queue[pool->task_tail].task_function = task_function;
        pool->task_queue[pool->task_tail].arg = arg;
        pool->task_tail = (pool->task_tail + 1) % TOTAL_TASKS;
        pool->task_count++;
        pthread_cond_signal(&pool->cond);
    } else {
        printf("Task queue is full!\n");
    }
    pthread_mutex_unlock(&pool->mutex);
}

void init_thread_pool(ThreadPool* pool) {
    pool->task_count = 0;
    pool->task_head = 0;
    pool->task_tail = 0;
    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->cond, NULL);

    for (int i = 0; i < TOTAL_THREADS; ++i) {
        pthread_create(&pool->threads[i], NULL, worker, (void*)pool);
    }
}

void destroy_thread_pool(ThreadPool* pool) {
    for (int i = 0; i < TOTAL_THREADS; ++i) {
        pthread_cancel(pool->threads[i]);
        pthread_join(pool->threads[i], NULL);
    }
}

void example_task(int num) {
    printf("Task: %d\n", num);
    usleep(500000);
}

int main() {
    ThreadPool pool;
    init_thread_pool(&pool);

    for (int i = 0; i < TOTAL_TASKS; ++i) {
        submit_task(&pool, example_task, i);
    }

    // Wait for tasks to finish using condition variable
    pthread_mutex_lock(&pool.mutex);
    while (pool.task_count > 0) {
        pthread_cond_wait(&pool.cond, &pool.mutex);
    }
    pthread_mutex_unlock(&pool.mutex);

    destroy_thread_pool(&pool);

    return 0;
}
