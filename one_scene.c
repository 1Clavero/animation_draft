 /* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   one_scene.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/26 03:04:30 by artclave          #+#    #+#             */
/*   Updated: 2024/02/17 11:00:45 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fractal.h"
#include <time.h>

void	initialize_mlx(t_mlx *mlx)
{
	mlx->mlx = mlx_init();
	mlx->height = 400;
	mlx->width = 400;
	mlx->window = mlx_new_window(mlx->mlx, mlx->width, mlx->height,"in between");
	mlx->image.img = mlx_new_image(mlx->mlx, mlx->width, mlx->height);
	mlx->n = 9; //16, 17, bakc to 9
	mlx->max.x = 0;
	mlx->max.y = 0;
	mlx->max.z = 0;
	mlx->frame = NULL;
	mlx->head = NULL;
}

int	lerp_colors(int	color_start, int color_end, double step)
{
	t_color	color;

	if (step > 1)
		step = 1;
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

void	ft_mlx_pixel_put(t_data *data, int x, int y, int color)
{
	char	*dst;

	dst = data->address + (y * data->line_length
			+ x * (data->bits_per_pixel / 8));
	*(unsigned int *)dst = color;
}

static int display_frames(t_mlx *mlx)
{
	if (!mlx->is_paused)
	{
		mlx_put_image_to_window(mlx->mlx, mlx->window, mlx->frame->image.img, 0, 0);
		usleep(200000);
//		usleep(20000000);
		if (mlx->frame->next)
			mlx->frame = mlx->frame->next;
	}
	return (1);
}

int key_press(int key, t_mlx *mlx)
{
    if (key == 36)
    {
        mlx->is_paused = 0;
        mlx_loop_hook(mlx->mlx, display_frames, mlx);
    }
    else if (key == 49)
        mlx->is_paused = 1;
    return 0;
}

t_list	*new_frame(t_mlx *mlx)
{
	t_list	*new;

	new = malloc(sizeof(t_list));
	if (!new)
		return (NULL);
	new->image.img = mlx_new_image(mlx->mlx, mlx->width, mlx->height);
	new->image.address = mlx_get_data_addr(new->image.img, &new->image.bits_per_pixel,
		&new->image.line_length, &new->image.endian);
	new->next = NULL;
	if (mlx->frame == NULL)
	{
		mlx->frame = new;
		mlx->head = new;
	}
	else
	{
		mlx->frame->next = new;
		mlx->frame = mlx->frame->next;
	}
	return (new);
}

void	initialize_function_array(void (*draw_scene[TOTAL_SCENES][2])(t_mlx *))
{
	draw_scene[0][BACKGROUND] = draw_bulb;
	draw_scene[0][FOREGROUND] = NULL;
}

void	initialize_zoom_values(t_scene	*scene, int n)
{

	//BULB
	scene->zoom.is_static[n][BACKGROUND] = NO;
	scene->zoom.value[n][BACKGROUND] = 0;
	scene->zoom.factor[n][BACKGROUND] = 0.001; //0.001
	scene->zoom.exponential[n][BACKGROUND] =  0.001; //0.001
	scene->zoom.speed_up_one[n][BACKGROUND] = 2; //2
	scene->zoom.stop[n][BACKGROUND] = 4; // 4
	//
	scene->end_determined_by[n] = BACKGROUND;
//HILBERT
}

int	has_scene_ended(t_scene *scene, int n, int layer_num)
{
	int	layer;

	if (layer_num != -1)
		layer = layer_num;
	else
		layer = scene->end_determined_by[n];
	if (scene->zoom.value[n][layer] >= scene->zoom.stop[n][layer]
			&& scene->zoom.factor[n][layer] > 0)
	{
		printf("scene has ended at %f positive direction\n", scene->zoom.value[n][layer]);
		return (YES);
	}
	if (scene->zoom.value[n][layer] <= scene->zoom.stop[n][layer]
			&& scene->zoom.factor[n][layer] < 0)
	{
		printf("scene has ended at %f negative direction\n", scene->zoom.value[n][layer]);
		return (YES);
	}
	return (NO);
}

double	get_zoom_value(t_scene *scene, int n, int layer)
{
	if (scene->draw_scene[n][layer] == NULL)
		return (-1);
	if (scene->zoom.is_static[n][layer] == 1)
		return (0);
	if (scene->zoom.value[n][layer] > scene->zoom.speed_up_one[n][layer]
			&& scene->zoom.factor[n][layer] > 0)
	{
		scene->zoom.factor[n][layer] += (double)(scene->zoom.exponential[n][layer] * 20);
		//printf("speed_up positive direction\n");
	}
	else if (scene->zoom.value[n][layer] < scene->zoom.speed_up_one[n][layer]
			&& scene->zoom.factor[n][layer] < 0)
	{
		scene->zoom.factor[n][layer] += (double)(scene->zoom.exponential[n][layer] * 20);
		//printf("speed_up negative direction\n");
	}

	else
		scene->zoom.factor[n][layer] += scene->zoom.exponential[n][layer];
	scene->zoom.value[n][layer] += scene->zoom.factor[n][layer];
	return (scene->zoom.value[n][layer]);
}

int get_pixel_color(t_data *data, int x, int y) {
    char *pixel_ptr = data->address + (y * data->line_length) + (x * (data->bits_per_pixel / 8));
    return *(unsigned int *)pixel_ptr;
}

void	get_in_between_frames(t_list *prev_frame,t_list *curr_frame, t_mlx *mlx)
{
	int	i;
	int total_lerps = 5;
	int color1;
	int	color2;

	int	x;
	int	y;

	i = -1;
	mlx->frame = prev_frame;
	while (++i < total_lerps)
	{
		new_frame(mlx);
		x = -1;
		while (++x < mlx->width)
		{
			y = -1;
			while (++y < mlx->height)
			{
				color1 = get_pixel_color(&prev_frame->image, x, y);
				color2 = get_pixel_color(&curr_frame->image, x, y);
				color1 = lerp_colors(color1, color2, i / total_lerps);
				ft_mlx_pixel_put(&mlx->frame->image, x, y, color1);
			}
		}
	}
	mlx->frame->next = curr_frame;
}

void	pre_render_frames_single_scene(t_mlx *mlx, t_scene *scene)
{
	int	scene_num;
	static int	i;
	t_list		*prev_frame;
	t_list		*curr_frame;

	initialize_zoom_values(scene, 0);
	initialize_function_array(scene->draw_scene);
	scene_num = 0;
	while (scene_num < TOTAL_SCENES) //2 scenes max is 1
	{
		curr_frame = new_frame(mlx);
		mlx->zoom = get_zoom_value(scene, scene_num, BACKGROUND);
		if (i % 2 != 0)
			get_in_between_frames(prev_frame, curr_frame, mlx);
		else
			prev_frame = curr_frame;
		i++;
	//	printf("mlx->zoom %f\n", mlx->zoom);
		scene->draw_scene[scene_num][BACKGROUND](mlx);
		mlx->zoom = get_zoom_value(scene, scene_num, FOREGROUND);
		if ((int)mlx->zoom != -1)
			scene->draw_scene[scene_num][FOREGROUND](mlx);
		if (has_scene_ended(scene, scene_num, -1) == YES)
			scene_num++;
	}
	mlx->frame = mlx->head;
}

int	main(void)
{
	t_mlx	mlx;
	t_scene	scene;

    clock_t start_time = 0, end_time = 0;
    double cpu_time_used;
	initialize_mlx(&mlx);
	pre_render_frames_single_scene(&mlx, &scene);
	printf("\nFrames rendered!\n");
	 end_time = clock();

    cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;

    printf("Execution Time: %f seconds\n", cpu_time_used);
	mlx_hook(mlx.window, KeyPress, KeyPressMask, key_press, &mlx);
	mlx_loop(mlx.mlx);
	return (0);
}
