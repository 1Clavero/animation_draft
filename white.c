/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   white.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/17 12:21:20 by artclave          #+#    #+#             */
/*   Updated: 2024/02/19 06:02:34 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fractal.h"

static void	ft_mlx_pixel_put(t_data *data, int x, int y, int color)
{
	char	*dst;

	dst = data->address + (y * data->line_length
			+ x * (data->bits_per_pixel / 8));
	*(unsigned int *)dst = color;
}

static void	plot_points(t_thread *args)
{
	int	x;
	int	y;

	y = args->first;
	while (++y < args->last)
	{
		x = -1;
		while (++x < args->mlx->width)
			ft_mlx_pixel_put(&args->mlx->frame->image, x, y, 0x0FFFFFF);
	}
}

void	draw_white(t_mlx *mlx)
{
		int			i;
	pthread_t	thread[12];
	t_thread	args[12];

	i = -1;
	while (++i < 12)
	{
		args[i].mlx = mlx;
		if (i == 0)
			args[i].first = (int)((i * (mlx->height / 12) -1));
		else
			args[i].first= args[i - 1].last - 1;
		args[i].last = (int)((i + 1) * ((mlx->height / 12) + 1));
		if (args[i].last > mlx->height)
			args[i].last = mlx->height;
		pthread_create(&thread[i], NULL, (void *(*)(void *))&plot_points, (void *)&args[i]);
	}
	i = -1;
	while (++i < 12)
		pthread_join(thread[i], NULL);
}

