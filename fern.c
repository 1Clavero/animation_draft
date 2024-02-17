/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fern.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 14:58:09 by artclave          #+#    #+#             */
/*   Updated: 2024/02/17 20:30:22 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fractal.h"
#include <complex.h>
#include "time.h"

static void	ft_mlx_pixel_put(t_data *data, int x, int y, int color)
{
	char	*dst;

	dst = data->address + (y * data->line_length
			+ x * (data->bits_per_pixel / 8));
	*(unsigned int *)dst = color;
}
/*
static void	initialize_mlx(t_mlx *mlx)
{
	mlx->mlx = mlx_init();
	mlx->height = 800;
	mlx->width = 800;
	mlx->window = mlx_new_window(mlx->mlx, mlx->width, mlx->height, "FERN");
	mlx->image.img = mlx_new_image(mlx->mlx, mlx->width, mlx->height);
	mlx->image.address = mlx_get_data_addr(mlx->image.img, &mlx->image.bits_per_pixel,
		&mlx->image.line_length, &mlx->image.endian);
}*/

static void	fern_transformation(t_coordinates *z)
{
	double	temp_x;

	if (z->rand < 0.01)
	{
		temp_x = 0.0;
		z->y = 0.16 * z->y;
	}
	else if (z->rand < 0.86)
	{
		temp_x = (0.85 * z->x) + (0.04 * z->y);
		z->y = (-0.04 * z->x) + (0.85 * z->y) + 1.6;
	}
	else if (z->rand < 0.93)
	{
		temp_x = (0.2 * z->x) - (0.26 * z->y);
		z->y = (0.23 * z->x) + (0.22 * z->y) + 1.6;
	}
	else
	{
		temp_x = (-0.15 * z->x) + (0.28 * z->y);
		z->y = (0.26 * z->x) + (0.24 * z->y) + 0.44;
	}
	z->x = temp_x;
}

static double	map (double z, double z_min, double z_range, double pixel_range)
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
/*
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

static void	draw_background(t_mlx *mlx)
{
	t_coordinates	p;

	p.x = -1;
	while (++p.x < mlx->width)
	{
		p.y = -1;
		while (++p.y < mlx->height)
			ft_mlx_pixel_put(&mlx->frame->image,p.x, p.y, 0x0000000);
	}
}
*/
static void	rotate(t_coordinates *p, double deg)
{
	double complex	xy;
	double	radians;

	radians = deg * (M_PI / 180);
	xy = (p->x - 800 / 2 + (I * (p->y - 800 /2))) * (cos(radians) + (I * sin(radians)));
	p->x = creal(xy) + 800/2;
	p->y = cimag(xy) + 800/2;
}

static void	plot_points(t_mlx *mlx, int iterations, int color, double deg)
{
	t_coordinates	z;
	t_coordinates	p;
	int	i;

	srand(time(NULL));
	z.x = 0;
	z.y = 0;
	i = -1;
	//if (mlx->zoom < 1)
	//	iterations *= 1 / mlx->zoom;
	//if (mlx->zoom > 2)
	//	iterations -= (int)(mlx->zoom * 5);
	iterations /= 1000; //10
	//printf("tierations: %d\n", iterations);
	while (++i < iterations)
	{

		z.rand = (rand() % 101);
		z.rand /= 100;
		fern_transformation(&z);
		p.x = map(z.x, mlx->range.x_min, mlx->range.x_total, mlx->width);
		p.y = map(z.y, mlx->range.y_min, mlx->range.y_total, mlx->height);
		rotate(&p, deg);
		if (p.x < 800 && p.y < 800 && p.x >= 0 && p.y >= 0)
			ft_mlx_pixel_put(&mlx->frame->image, p.x, p.y, color);
			//0x043927
	//	draw_fern(z);
	}
}

static void	set_fractal_range(t_mlx *mlx)
{
	mlx->range.x_max = 10 * mlx->zoom + 1;
	mlx->range.y_max = 10 * mlx->zoom + 0.75;
	mlx->range.x_min = -10 * mlx->zoom + 1;
	mlx->range.y_min = -10 * mlx->zoom + 0.75;
	mlx->range.x_total = mlx->range.x_max - mlx->range.x_min;
	mlx->range.y_total = (mlx->range.y_max - mlx->range.y_min) * - 1;
}

void	draw_fern(t_mlx *mlx)
{
	int	deg;
	//if (mlx->zoom < 0.01)
	//	mlx->zoom += mlx->zoom;
	//else

	/*if (mlx->zoom > 1)
		mlx->zoom += 0.01;
	else
		mlx->zoom += 0.001;
		*/
	set_fractal_range(mlx);
	//draw_background(mlx);
	deg = 45;
	while (deg < 360)
	{
		plot_points(mlx, 5000000, 0x0417822, deg);
		plot_points(mlx, 50000, 0x087e019, deg);
		plot_points(mlx, 5000, 0x0a9e028, deg);
		plot_points(mlx, 500, 0x0c9e034, deg);
		deg += 90;
	}
//	ft_mlx_pixel_put(&mlx->image, 400, 400, 0x0FF0000); //0x043927
	//mlx_put_image_to_window(mlx->mlx, mlx->window, mlx->image.img, 0, 0);
}
