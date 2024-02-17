/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   white.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/17 12:21:20 by artclave          #+#    #+#             */
/*   Updated: 2024/02/17 12:37:31 by artclave         ###   ########.fr       */
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

void	draw_white(t_mlx *mlx)
{
	int	x;
	int	y;

	x = -1;
	while (++x < mlx->width)
	{
		y = -1;
		while (++y < mlx->height)
			ft_mlx_pixel_put(&mlx->frame->image, x, y, 0x0FFFFFF);
	}
}
