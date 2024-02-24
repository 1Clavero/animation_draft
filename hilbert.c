/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hilbert.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/15 01:42:49 by artclave          #+#    #+#             */
/*   Updated: 2024/02/20 16:35:26 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "fractal.h"
#include "time.h"
# include <pthread.h>

static double	power_of(double base, double exponent)
{
	int	i;
	double result;

	if (exponent == 0)
		return (1);
	if (exponent == 1)
		return (base);
	result = base;
	i = 0;
	while (++i < exponent)
		result *= base;
	return (result + 1e-9);
}

static void	ft_mlx_pixel_put(t_data *data, int x, int y, int color)
{
	char	*dst;

	dst = data->address + (y * data->line_length
			+ x * (data->bits_per_pixel / 8));
	*(unsigned int *)dst = color;
}

static int	calculate_order(double map_width)
{
	int n;
	int order;

	n = 1;
	order = 1;
	while ((double)n < map_width)
	{
		n *= 2;
		order++;
	}
	if (order % 2 == 0)
		order++;
	return (order);
}

static void	set_fractal_range(t_mlx *mlx)
{
	mlx->range.x_max = 2 * mlx->zoom + 0.8;
	mlx->range.y_max = 2 * mlx->zoom + 0.2;
	mlx->range.x_min = 0 * mlx->zoom + 0.8;
	mlx->range.y_min = 0 * mlx->zoom + 0.2;
	mlx->range.x_total = mlx->range.x_max - mlx->range.x_min;
	mlx->range.y_total = (mlx->range.y_max - mlx->range.y_min) * -1;
	mlx->order = calculate_order(mlx->range.x_total);
	if (mlx->order < 5)
		mlx->order = 5;
	mlx->n = (int)pow(2, mlx->order);
	mlx->points = (int)mlx->n * mlx->n;
	mlx->one = 1;
	mlx->zero = mlx->one / 2;
}

static double	map(double z, double z_min, double z_range, double pixel_range)
{
	double	pixel;

	z -= z_min;
	if (z_range > 0)
		pixel = (z / z_range) * pixel_range;
	else
	{
		z_range *= -1;
		pixel = (1 - (z / z_range)) * pixel_range;
	}
	return (pixel);
}

static void	starting_points(t_coordinates *new, int point, t_mlx *mlx)
{
	if (point == 0)
	{
		new->x = mlx->zero;
		new->y = mlx->zero;
	}
	if (point == 1)
	{
		new->x = mlx->zero;
		new->y = mlx->zero + mlx->one;
	}
	if (point == 2)
	{
		new->x = mlx->zero + mlx->one;
		new->y = mlx->zero + mlx->one;
	}
	if (point == 3)
	{
		new->x = mlx->zero + mlx->one;
		new->y =  mlx->zero;
	}
}

static void	get_quadrant(t_coordinates *new, int quad, t_mlx *mlx)
{
	double	temp;

	if (quad == 0 && mlx->order > 1)
	{
		temp = new->x;
		new->x = new->y;
		new->y = temp;
	}
	if (quad == 1 || quad == 2)
		new->y += (mlx->len * 2);
	if (quad == 2)
		new->x += (mlx->len * 2);
	if (quad == 3)
	{
		temp = mlx->len * 2  - new->x;
		new->x = mlx->len * 2  - new->y + (mlx->len * 2);
		new->y = temp;
	}
}

static void	get_points(t_coordinates *new, t_mlx *mlx, int i)
{
	int	j;

	j = 1;
	while (++j <= mlx->order)
	{
		mlx->len = mlx->one * pow(2, (j - 2));
		i = i >> 2;
		get_quadrant(new, i & 3, mlx);
	}
}

static int	color_pixel(t_mlx *mlx, t_coordinates p, int color)
{
	int	x;
	int	y;

	x = (int)p.x;
	y = (int)p.y;
	if (color == 0x0FFFFFF)
		mlx->grid[x][y] = color;
	else if (color == 0x00000FF)
	{
		if (mlx->grid[x][y] == 0x0FFFFFF)
			return (0);
		mlx->grid[x][y] = color;
	}
	else if (color == 0x0FF0000)
	{
		if (mlx->grid[x][y] == 0x0FFFFFF)
			return (0);
		mlx->grid[x][y] = color;
	}
	return (1);
}

static void	draw_line(t_mlx *mlx, t_coordinates z, t_coordinates new, int color)
{
	t_coordinates	distance;
	t_coordinates	increment;
	double	i;

	z.x = map(z.x, mlx->range.x_min, mlx->range.x_total, mlx->width);
	z.y = map(z.y, mlx->range.y_min, mlx->range.y_total, mlx->height);
	new.x = map(new.x, mlx->range.x_min, mlx->range.x_total, mlx->width);
	new.y = map(new.y, mlx->range.y_min, mlx->range.y_total, mlx->height);
	distance.x = fabs(new.x - z.x);
	distance.y = fabs(new.y - z.y);
	if (distance.x > distance.y)
	{
		if (z.x - new.x > 0)
			increment.x = 1;
		else
			increment.x = -1;
		increment.y = 0;
	}
	else
	{
		if (z.y - new.y > 0)
			increment.y = 1;
		else
			increment.y = -1;
		increment.x = 0;
	}
	i = -1;
	while (++i < distance.x || i < distance.y)
	{

		if (new.x < 800 && new.y < 800 && new.x > 0 && new.y > 0)
		{
			if (!color_pixel(mlx, new, color))
				return ;
		}
		new.x += increment.x;
		new.y += increment.y;
	}
}

static void	draw_top_surface(t_mlx *mlx, t_coordinates start, t_coordinates end)
{
	double	temp_end;
	double	temp_start;
	int	i;
	int	iterations;
	double	width;

	int	color;
	color = 0x0FFFFFF;
	iterations = 300;
	width = 0.001;
	if (mlx->zoom < 0.6)
	{
			iterations *= 10;
			width /= 10;
	}
	i = iterations * -1;
	if (start.y == end.y)
	{
		end.x += 0.299;
		temp_start = start.y;
		temp_end = end.y;
		while (++i < iterations)
		{
			start.y = temp_start + (i * width);
			end.y = temp_end + (i * width);
			draw_line(mlx, start, end, color);
		}
	}
	if (start.x == end.x)
	{
		end.y += 0.299;
		temp_start = start.x;
		temp_end = end.x;
		while (++i < iterations)
		{
			end.x = temp_end + (i * width);
			start.x = temp_start + (i * width);
			draw_line(mlx, start, end, color);
		}
	}
}

void	draw_side_surface(t_mlx *mlx, t_coordinates z, t_coordinates new, int color)
{
	int	i;
	int	iterations;
	double	width;

	i = -1;
	iterations = 150;
	width = 0.001;
	if (mlx->zoom < 0.6)
	{
		iterations *= 10;
		width /= 10;
	}
	while (++i < iterations)
	{
		z.x -= width;
		new.x -= width;
		z.y -= width;
		new.y -= width;
		draw_line(mlx, z, new, color);
	}
}

static void	draw_vertical_line(t_mlx *mlx, t_coordinates start, t_coordinates end)
{
	draw_top_surface(mlx, start, end);
	end.y -= 0.299;
	start.x -= 0.299;
	end.x -= 0.299;
	draw_side_surface(mlx, start, end, 0x00000FF);
	end.x += (0.299 * 2);
	end.y = start.y;
	draw_side_surface(mlx, start, end, 0x0FF0000);
}

static void	draw_horizontal_line(t_mlx *mlx, t_coordinates start, t_coordinates end)
{
	draw_top_surface(mlx, start, end);
	end.x -= 0.299;
	start.y -= 0.299;
	end.y = start.y;
	draw_side_surface(mlx, start, end, 0x0FF0000);
	end.y += (2 * 0.299);
	end.x = start.x;
	draw_side_surface(mlx, start, end, 0x00000FF);
}

static void	draw_surfaces(t_mlx *mlx, t_coordinates z, t_coordinates new)
{
	if (z.x == new.x && z.y < new.y)
	{
		z.y -= 0.299;
		draw_vertical_line(mlx, z, new);
	}
	else if (z.x == new.x && new.y < z.y)
	{
		new.y -= 0.299;
		draw_vertical_line(mlx, new, z);
	}
	if (z.y == new.y && z.x < new.x)
	{
		z.x -= 0.299;
		draw_horizontal_line(mlx, z, new);
	}
	else if (z.y == new.y && new.x < z.x)
	{
		new.x -= 0.299;
		draw_horizontal_line(mlx, new, z);
	}
}

static void	points_to_image(t_mlx *mlx)
{
	int	i;
	int	j;

	i = -1;
	while (++i < 800)
	{
		j = -1;
		while (++j < 800)
			ft_mlx_pixel_put(&mlx->frame->image, i, j, mlx->grid[i][j]);
	}
}

static void	plot_points(t_thread *args)
{
	t_coordinates	z;
	t_coordinates	new;
	int	i;

	i = args->first;
	z.x = args->x;
	//printf("args->first: %d, args->last: %d\n", args->first, args->last);
	while (++i < args->last)
	{

		starting_points(&new, i & 3, args->mlx);
		get_points(&new, args->mlx, i);
		draw_surfaces(args->mlx, z, new);
		z.x = new.x;
		z.y = new.y;
	}
	args->x = z.x;
	points_to_image(args->mlx);
}

static void	draw_background(t_mlx *mlx)
{
	int	i;
	int	j;

	i = -1;
	while (++i < 800)
	{
		j = -1;
		while (++j < 800)
			mlx->grid[i][j] = 0x0000000;
	}
}

void	draw_hilbert(t_mlx *mlx)
{
	int			i;
	pthread_t	thread[12];
	t_thread	args[12];
	int			total;

	mlx->zoom += 0.1;
	set_fractal_range(mlx);
	draw_background(mlx);
	i = -1;
	total = 0;
	while (++i < 12)
	{
		args[i].mlx = mlx;
		if (i == 0)
		{
			args[i].first = -1;
			args[i].x = -1;
		}
		else
		{
			args[i].first = args[i - 1].last - 1;
			args[i].x = args[i - 1].x;
		}
		args[i].last = args[i].first + (mlx->points / 12) + 2;
		if (i == 11 && args[i].last != mlx->points)
			args[i].last = mlx->points;
		total += args[i].last - args[i].first - 1;
		pthread_create(&thread[i], NULL, (void *(*)(void *))&plot_points, (void *)&args[i]);
	}
	i = -1;
	while (++i < 12)
		pthread_join(thread[i], NULL);
}
