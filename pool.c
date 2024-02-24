/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pool.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/21 18:32:31 by artclave          #+#    #+#             */
/*   Updated: 2024/02/22 17:04:55 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#define FALSE -1
#define TOTAL_ROWS 100

typedef struct	s_thread
{
	int				(*rows)[TOTAL_ROWS];
	pthread_cond_t	*cond_task_due;
	int				*all_task_sent;
	pthread_mutex_t	*mutex_queue;
}			t_thread;

typedef struct	s_values
{
	int				rows[TOTAL_ROWS];
	pthread_cond_t	cond_task_due;
	int				all_task_sent;
	pthread_mutex_t	mutex_queue;
}			t_values;

void	execute_task(int x)
{
	printf("row: %d\n", x);
	usleep(500000);
}

void	execute_thread(t_thread *args)
{
	int	i;

	while (! ((*args->rows)[0] == -1 && *(args->all_task_sent)))
	{
		pthread_mutex_lock(args->mutex_queue);
		while ((*args->rows)[0] == -1)
		{
			if (*(args->all_task_sent))
				break;
			pthread_cond_wait(args->cond_task_due, args->mutex_queue);
		}
		execute_task((*args->rows)[0]);
		i = 0;
		while(++i < TOTAL_ROWS && (*args->rows)[i - 1] != -1)
			(*args->rows)[i - 1] = (*args->rows)[i];
		(*args->rows)[i] = -1;
		pthread_mutex_unlock(args->mutex_queue);
	}
	pthread_exit(NULL);
}

void	create_task(t_thread *args)
{
	static int	row_num;
	int			last_row;

	pthread_mutex_lock(args->mutex_queue);
	last_row = 0;
	while (last_row < TOTAL_ROWS && (*args->rows)[last_row] != -1)
		last_row++;
	((*args->rows)[last_row]) = row_num;
	row_num++;
	if (row_num == TOTAL_ROWS)
	{
		row_num = 0;
		(*args->all_task_sent) = 1;
	}
	pthread_mutex_unlock(args->mutex_queue);
	pthread_cond_broadcast(args->cond_task_due);
}

void	init_args(t_thread *dst, t_values *src)
{
	dst->all_task_sent = &(src->all_task_sent);
	dst->cond_task_due = &(src->cond_task_due);
	dst->mutex_queue = &(src->mutex_queue);
	dst->rows = &(src->rows);
}

void	init_thread_values(t_values *init_values)
{
	pthread_mutex_t	mutex_queue;
	pthread_cond_t	cond_task_due;
	int				all_tasks_sent;
	int				i;

	pthread_mutex_init(&mutex_queue, NULL);
	pthread_cond_init(&cond_task_due, NULL);
	i = -1;
	while(++i < TOTAL_ROWS)
		init_values->rows[i] = -1;
	all_tasks_sent = 0;
	i = -1;
	init_values->all_task_sent = all_tasks_sent;
	init_values->cond_task_due = cond_task_due;
	init_values->mutex_queue = mutex_queue;
}

int	main(void)
{
	pthread_t	thread[12];
	t_thread	args[12];
	t_values	init_values;
	int			i;

	init_thread_values(&init_values);
	i = -1;
	while (++i < 12)
		init_args(&args[i], &init_values);
	i = -1;
	printf("check\n");
	while (++i < 12)
	{
		pthread_create(&thread[i], NULL, (void *(*)(void *))execute_thread, (void *)(&args[i]));
	}
	i = -1;
	while (++i < TOTAL_ROWS)
		create_task(&args[0]);
	i = -1;
	while (++i < 12)
		pthread_join(thread[i], NULL);
	pthread_mutex_destroy(args[0].mutex_queue);
	pthread_cond_destroy(args[0].cond_task_due);
}
