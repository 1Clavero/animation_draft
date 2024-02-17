/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fractal1.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/19 11:02:45 by artclave          #+#    #+#             */
/*   Updated: 2024/02/13 10:36:29 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FRACTAL_H
# define FRACTAL_H

# include "mlx.h"
# include <stdio.h>
# include <stdlib.h>
# include <math.h>



typedef	struct	s_color
{
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
}				t_data;

typedef struct	s_range
{
	double	x_max;
	double	x_min;
	double	y_min;
	double	y_max;
	double	x_total;
	double	y_total;
}			t_range;

typedef struct	s_coordinates
{
	double	x;
	double	y;
	double	rand;
	int	iterations;
}
	t_coordinates;

typedef struct	s_mlx
{
	void	*mlx;
	void	*window;
	t_data	image;
	double	height;
	double	width;
	t_range	range;
}		t_mlx;

#endif
//cc test.c -L. -lmlx -framework OpenGL -framework AppKit
