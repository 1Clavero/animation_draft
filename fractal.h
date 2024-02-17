/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fractal.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/19 11:02:45 by artclave          #+#    #+#             */
/*   Updated: 2024/02/17 21:14:28 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FRACTAL_H
# define FRACTAL_H

# include "mlx.h"
# include <stdio.h>
# include <stdlib.h>
# include <complex.h>
# include <math.h>
# include <stdarg.h>
# include <unistd.h>
# include <X11/Xlib.h>
# include <X11/X.h>
# include <X11/keysym.h>
# include <pthread.h>

# define FOREGROUND 1
# define BACKGROUND 0
# define NO 0
# define YES 1
# define TOTAL_SCENES 4


typedef	struct	s_color
{
	int	*palette;
	int	palette_size;
	double	palette_ratio;
	int	total_lerps;
	double	lerp_step;
	int	r1;
	int	r2;
	int	r_new;
	int	g1;
	int	g2;
	int	g_new;
	int	b1;
	int	b2;
	int	b_new;
}		t_color;

typedef struct	s_data
{
	void	*img;
	char	*address;
	int		bits_per_pixel;
	int		line_length;
	int		endian;
//	int		zoom;
}				t_data;

typedef struct	s_range
{
	double	x_max;
	double	x_min;
	double	y_min;
	double	y_max;
	double	x_total;
	double	y_total;
	double	max;
	double	min;
	double	total;
}			t_range;

typedef struct	s_coordinates
{
	double	x;
	double	y;
	double	z;
	double rand;
	int	color;
	int	iterations;
}
	t_coordinates;

typedef struct s_noise
{
	int	octaves;
	double	frequency;
	double	amplitude;
	double	grid_size;
	int	color_start;
	int	color_end;
	double	color_step;
	double	min_distance;
	double	adj_distance;
}			t_noise;

typedef struct	s_list
{
	t_data	image;
	int	num;
	struct s_list	*next;
}			t_list;


typedef struct	s_mlx
{
	void	*mlx;
	void	*window;
	int	height;
	int	width;
	t_range	range;
	t_range limits;
	t_data	image;
	t_list	*frame;
	t_list	*head;
	double	zoom;
	int		is_paused;
	double	c_values[4][2];
	t_coordinates	c;
	char	set;
	double	shift_x;
	double	shift_y;
	t_color	color;
	int	order;
	int	n;
	int	points;
	double	one;
	double	zero;
	double	len;
	double	double_height;
	double	double_width;
	int	grid[800][800];
}		t_mlx;

typedef	struct	s_zoom
{
	double	value[TOTAL_SCENES][2];
	double	stop[TOTAL_SCENES][2];
	double	factor[TOTAL_SCENES][2];
	double	exponential[TOTAL_SCENES][2];
	double	speed_up_one[TOTAL_SCENES][2];
	double	is_static[TOTAL_SCENES][2];
}			t_zoom;

typedef struct	s_scene
{
	void	(*draw_scene[TOTAL_SCENES][2])(t_mlx *);
	t_zoom	zoom;
	int		end_determined_by[TOTAL_SCENES];
}			t_scene;


void	draw_mandelbrot(t_mlx *mlx);
void	draw_white(t_mlx *mlx);
void	draw_cells(t_mlx *mlx);
void	draw_fern(t_mlx *mlx);
void	draw_hilbert(t_mlx *mlx);
#endif
//if (pixel.x >= mlx->limits.min && pixel.y >= mlx->limits.min && pixel.x < mlx->limits.max && pixel.x < mlx->limits.max);
//				ft_mlx_pixel_put(&mlx->frame->image, pixel.x, pixel.y, pixel.color);
//gcc -Wall -Wextra -Werror one_scene.c fern.c -L. -lmlx -framework OpenGL -framework AppKit
//gcc -Wall -Wextra -Werror test.c mandelbrot.c -L. -lmlx -framework OpenGL -framework AppKit
//gcc -Wall -Wextra -Werror test.c mandelbrot.c white.c cells.c fern.c -L. -lmlx -framework OpenGL -framework AppKit
