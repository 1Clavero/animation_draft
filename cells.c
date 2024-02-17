/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cells.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/21 18:46:05 by artclave          #+#    #+#             */
/*   Updated: 2024/01/26 08:44:12 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fractal.h"
#include <time.h>

void	ft_mlx_pixel_put(t_data *data, int x, int y, int color)
{
	char	*dst;

	dst = data->address + (y * data->line_length
			+ x * (data->bits_per_pixel / 8));
	*(unsigned int *)dst = color;
}

double	map(double pixel, double total_pixels, double fractal_min, double fractal_max)
{
	double	result;

	result = fractal_min + ((fractal_max - fractal_min) / (total_pixels - 1) * pixel);
	return (result);
}

int	lerp_colors(int	color_start, int color_end, double step)
{
	t_color	color;

	if (step > 1)
		step = 1;
	if (step < 0)
		step = 0;
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


void	hash(double x, double y, t_coordinates *v)
{
	v->x = sin((x * 127.1) + (y * 311.7));
	v->y = sin((x * 269.5) + (y * 183.3));
	v->x = v->x - floor(v->x);
	v->y = v->y - floor(v->y);
	v->x += x;
	v->y += y;
}

double	distance_next_point(double grid_x, double grid_y, t_coordinates point, t_mlx *mlx)
{
	t_coordinates	grid_vector;
	double	distance;

	hash(grid_x, grid_y, &grid_vector);
	if (!((point.x < 0 && point.y < 0) || (point.x >= 0 && point.y >= 0)))
	{
		if (point.x < point.y)
		{
			point.x += point.x;
			point.y += point.y;
		}
		else
		{
			point.x += point.y;
			point.y += point.x;
		}
	}
	distance = sqrt((grid_vector.x - point.x) * (grid_vector.x - point.x)
			+ (grid_vector.y - point.y) * (grid_vector.y - point.y));
	return (distance);
}

void	worley_value(t_coordinates point, t_noise *noise, t_mlx *mlx)
{
	t_coordinates	grid;
	double	i;
	double	j;

	grid.x = (double)((int)point.x);
	grid.y = (double)((int)point.y);
	noise->min_distance = 1000000;
	i = -2;
	while (++i < 2)
	{
		j = -2;
		while (++j < 2)
		{
			noise->adj_distance = distance_next_point(grid.x + i, grid.y + j, point, mlx);
			if (noise->adj_distance < noise->min_distance)
				noise->min_distance = noise->adj_distance;
		}
	}
	noise->color_step = noise->min_distance / 0.8;
}

void	plot_points(t_mlx *mlx)
{
	t_noise	noise;
	t_coordinates	pixel;
	t_coordinates	z;

	noise.grid_size = 9;
	pixel.x = -1;
	while (++pixel.x < mlx->width)
	{
		z.x = map(pixel.x, mlx->width, mlx->range.x_min, mlx->range.x_max);
		pixel.y = -1;
		while (++pixel.y < mlx->height)
		{
			z.y = map(pixel.y, mlx->height, mlx->range.y_min, mlx->range.y_max);
			worley_value(z, &noise, mlx);
			pixel.color = lerp_colors(0x0000000, 0x0FFFFFF, noise.color_step);
			if (mlx->zoom < 4)
			{
				noise.color_step = (mlx->zoom) / 4;
				pixel.color = lerp_colors(0x09CC702, pixel.color, noise.color_step);
			//	pixel.color = lerp_colors(0x000A86B, pixel.color, noise.color_step);
			}
			ft_mlx_pixel_put(&mlx->frame->image, pixel.x, pixel.y, pixel.color);
		}
	}
}

static void	set_fractal_range(t_mlx *mlx)
{
	mlx->range.x_max = (19.35 - mlx->zoom);
	mlx->range.y_max = (21 - mlx->zoom);
	mlx->range.x_min = (-0.15 + mlx->zoom);
	mlx->range.y_min = (1 + mlx->zoom);
}

void	draw_cells(t_mlx *mlx)
{
	set_fractal_range(mlx);
	plot_points(mlx);
}
