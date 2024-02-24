/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mandelbrot_threads.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/21 08:17:46 by artclave          #+#    #+#             */
/*   Updated: 2024/02/19 05:23:21 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fractal.h"

static double	map(double pixel, double total_pixels, double fractal_min, double fractal_max)
{
	double	result;

	result = fractal_min + ((fractal_max - fractal_min) / (total_pixels - 1) * pixel);
	return (result);
}

static int	lerp_colors(int	color_start, int color_end, double step)
{
	t_color	color;

	if (step > 0.98)
	{
		color_start = 0x00000FF;
		color_end = 0x00000AA;
	}
	color.r1 = (color_start >> 16) & 0xFF;
	color.g1 = (color_start >> 8) & 0xFF;
	color.b1 = (color_start) & 0xFF;
	color.r2 = (color_end >> 16) & 0xFF;
	color.g2 = (color_end >> 8) & 0xFF;
	color.b2 = (color_end) & 0xFF;
	color.r_new = (int)((color.r1 * (1 - step)) + (color.r2 * step));
	color.g_new = (int)((color.g1 * (1 - step)) + (color.g2 * step));
	color.b_new = (int)((color.b1 * (1 - step)) + (color.b2 * step));
	return (color.r_new << 16 | color.g_new << 8 | color.b_new);
}

static double	complex_hypotenuse(t_coordinates *z, t_coordinates c)
{
	double	temp;
	double	hypo;

	temp = (z->x * z->x) - (z->y * z->y) + c.x;
	z->y = ((2 * z->x * z->y) + c.y);
	z->x = temp;
	hypo = (z->x * z->x) + (z->y * z->y);
	return (hypo);
}

static int	is_mandelbrot(t_coordinates c)
{
	t_coordinates	z;
	double	i;

	i = -1.0;
	z.iterations = 20;
	z.x = 0;
	z.y = 0;
	while (++i < z.iterations)
	{
		if (complex_hypotenuse(&z, c) > 4)
			break;
	}
	if (i == z.iterations)
		return (0x0000000);
	return (lerp_colors(0x0FFFFFF, 0x0000000, i / z.iterations));
}

static void	ft_mlx_pixel_put(t_data *data, int x, int y, int color)
{
	char	*dst;

	dst = data->address + (y * data->line_length
			+ x * (data->bits_per_pixel / 8));
	*(unsigned int *)dst = color;
}

static void	set_fractal_range(t_mlx *mlx)
{
	mlx->range.x_max = (2 * mlx->zoom);
	mlx->range.y_max = (2 * mlx->zoom);
	mlx->range.x_min = (-2 * mlx->zoom);
	mlx->range.y_min = (-2 * mlx->zoom);
}

static void	plot_points(t_thread *args)
{
	t_coordinates	pixel;
	t_coordinates	c;
	int				max_height;

	pixel.y = (double)args->first;
	max_height = (double)args->last;
	while (++pixel.y < max_height)
	{
		c.y = map(pixel.y, args->mlx->height, args->mlx->range.y_max, args->mlx->range.y_min);
		pixel.x = -1;
		while (++pixel.x < args->mlx->width)
		{
			c.x = map(pixel.x, args->mlx->width, args->mlx->range.x_min, args->mlx->range.x_max);
			pixel.color = is_mandelbrot(c);
			if (pixel.color == 0x0FFFFFF && args->mlx->zoom > 1.3)
				continue;
			if (args->mlx->zoom > 50)
				pixel.color = lerp_colors(pixel.color, 0x0FFFFFF, (args->mlx->zoom - 50) / 40);
			if (c.x > (-2 * 1.3) && c.x < (2 * 1.3) && c.y > (-2 * 1.3) && c.y <  (2 * 1.3))
				ft_mlx_pixel_put(&args->mlx->frame->image, pixel.x, pixel.y, pixel.color);
		}
	}
}

void	draw_mandelbrot(t_mlx *mlx)
{
	int			i;
	pthread_t	thread[12];
	t_thread	args[12];

	set_fractal_range(mlx);
	i = -1;
	while (++i < 12)
	{
		args[i].mlx = mlx;
		if (i == 0)
			args[i].first = (int)((i * (mlx->height / 12) -1));
		else
			args[i].first = args[i - 1].last - 1;
		args[i].last= (int)((i + 1) * ((mlx->height / 12) + 1));
		if (args[i].last > mlx->height)
			args[i].last = mlx->height;
		pthread_create(&thread[i], NULL, (void *(*)(void *))&plot_points, (void *)&args[i]);
	}
	i = -1;
	while (++i < 12)
		pthread_join(thread[i], NULL);
}

