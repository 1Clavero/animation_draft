#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define TOTAL_ROWS 100
#define TOTAL_THREADS 1

typedef struct s_thread
{
    int *rows;
    pthread_cond_t *cond_task_due;
    int *all_task_sent;
    pthread_mutex_t *mutex_queue;
} t_thread;

typedef struct s_values
{
    int rows[TOTAL_ROWS];
    pthread_cond_t cond_task_due;
    int all_task_sent;
    pthread_mutex_t mutex_queue;
} t_values;

void execute_task(int x)
{
    printf("row: %d\n", x);
    usleep(500000);
}

void execute_thread(t_thread *args)
{
    int i;

    while (!(args->rows[0] == -1 && *(args->all_task_sent)))
    {
        pthread_mutex_lock(args->mutex_queue);
        while (*(args->rows) == -1)
        {
            if (*(args->all_task_sent))
                break;
            pthread_cond_wait(args->cond_task_due, args->mutex_queue);
        }
		if (*(args->rows) != -1)
			execute_task(*(args->rows));
        for (i = 0; i < TOTAL_ROWS && args->rows[i] != -1; i++)
            args->rows[i] = args->rows[i + 1];
        args->rows[i] = -1;
        pthread_mutex_unlock(args->mutex_queue);
    }
    pthread_exit(NULL);
}

void create_task(t_thread *args)
{
    static int row_num = 0;

    pthread_mutex_lock(args->mutex_queue);
    int i = 0;
    while (args->rows[i] != -1 && i < TOTAL_ROWS)
        i++;
    if (i < TOTAL_ROWS)
    {
        args->rows[i] = row_num++;
        if (row_num == TOTAL_ROWS -1)
        {
          //  row_num = 0;
            *(args->all_task_sent) = 1;
        }
    }
    pthread_mutex_unlock(args->mutex_queue);
    pthread_cond_broadcast(args->cond_task_due);
}

void init_args(t_thread *dst, t_values *src)
{
    dst->all_task_sent = &(src->all_task_sent);
    dst->cond_task_due = &(src->cond_task_due);
    dst->mutex_queue = &(src->mutex_queue);
    dst->rows = src->rows;
}

void init_thread_values(t_values *init_values)
{
    pthread_mutex_t mutex_queue = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond_task_due = PTHREAD_COND_INITIALIZER;
    int i;

    for (i = 0; i < TOTAL_ROWS; i++)
        init_values->rows[i] = -1;

    init_values->all_task_sent = 0;
    init_values->cond_task_due = cond_task_due;
    init_values->mutex_queue = mutex_queue;
}

int main(void)
{
    pthread_t thread[TOTAL_THREADS];
    t_thread args[TOTAL_THREADS];
    t_values init_values;
    int i;

    init_thread_values(&init_values);
    for (i = 0; i < TOTAL_THREADS; i++)
    {
        init_args(&args[i], &init_values);
    }

    printf("check\n");
    for (i = 0; i < TOTAL_THREADS; i++)
    {
        pthread_create(&thread[i], NULL, (void *(*)(void *))execute_thread, (void *)&args[i]);
    }

    for (i = 0; i < TOTAL_ROWS; i++) // Changed to TOTAL_ROWS - 1
    {
        create_task(&args[0]);
    }

    for (i = 0; i < TOTAL_THREADS; i++)
    {
        pthread_join(thread[i], NULL);
    }

    pthread_mutex_destroy(&init_values.mutex_queue);
    pthread_cond_destroy(&init_values.cond_task_due);

    return 0;
}
