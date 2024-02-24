/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pool2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 16:58:15 by artclave          #+#    #+#             */
/*   Updated: 2024/02/22 17:15:04 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define TOTAL_ROWS 100
#define	TOTAL_THREADS 12

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

    while (!(*(args->rows) == -1 && *(args->all_task_sent)))
    {
        pthread_mutex_lock(args->mutex_queue);
        while (*(args->rows) == -1)
        {
            if (*(args->all_task_sent))
                break;
            pthread_cond_wait(args->cond_task_due, args->mutex_queue);
        }
        execute_task(*(args->rows));
		i = -1;
		while(++i < TOTAL_ROWS -1 && args->rows[i] != -1)
           *(args->rows)[i] = *(args->rows)[i + 1];
        *(args->rows)[TOTAL_ROWS - 1] = -1;
        pthread_mutex_unlock(args->mutex_queue);
    }
    pthread_exit(NULL);
}
/*
void create_task(t_thread *args)
{
	static int row_num;
	int	i;

	row_num = 0;
	i = 0;
	pthread_mutex_lock(args->mutex_queue);
	int i = 0;
	while (args->rows[i] != -1 && i < TOTAL_ROWS)
		i++;
	args->rows = i + 1;
	if (row_num == TOTAL_ROWS)
	{
		row_num = 0;
		*(args->all_task_sent) = 1;
	}
	pthread_mutex_unlock(args->mutex_queue);
	pthread_cond_broadcast(args->cond_task_due);
}*/

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
        init_values->rows[i] = i;
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
	i = -1;
	while (i++ < TOTAL_THREADS)
	{
		init_args(&args[i], &init_values);
	}
	i = -1;
	while (i++ < TOTAL_THREADS)
	{
		pthread_create(&thread[i], NULL, (void *(*)(void *))execute_thread, (void *)&args[i]);
	}
	/*i = -1;
	while (i++ < TOTAL_THREADS)
	{
		create_task(&args[0]);
	}*/
	i = -1;
	while (i++ < TOTAL_THREADS)
	{
		pthread_join(thread[i], NULL);
	}
	pthread_mutex_destroy(&init_values.mutex_queue);
	pthread_cond_destroy(&init_values.cond_task_due);
	return 0;
}
