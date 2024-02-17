 /* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                            :+:      :+:    :+:   */
/*                                                     +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/26 03:04:30 by artclave          #+#    #+#             */
/*   Updated: 2024/02/17 11:00:45 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fractal.h"
#include <time.h>
/*
void	display_menu(t_mlx *mlx, char *image_path)
{
	t_list	*menu;

	menu = malloc(sizeof(t_list));
	if (!menu)
		return ;
    menu->image.img = mlx_xpm_file_to_image(mlx->mlx, image_path, &mlx->width, &mlx->height);
    menu->image.address = mlx_get_data_addr(menu->image.img, &menu->image.bits_per_pixel,
                                                      &menu->image.line_length, &menu->image.endian);
	mlx->frame = menu;
    mlx_put_image_to_window(mlx->mlx, mlx->window, mlx->frame->image.img, 0, 0);
	mlx->head = menu;
}*/
void	initialize_mlx(t_mlx *mlx)
{
	mlx->mlx = mlx_init();
	mlx->height = 800;
	mlx->width = 800;
	mlx->window = mlx_new_window(mlx->mlx, mlx->width, mlx->height, "FULL");
	mlx->image.img = mlx_new_image(mlx->mlx, mlx->width, mlx->height);
	mlx->frame = NULL;
	mlx->head = NULL;
}

static int display_frames(t_mlx *mlx)
{
	if (!mlx->is_paused)
	{
		mlx_put_image_to_window(mlx->mlx, mlx->window, mlx->frame->image.img, 0, 0);
		usleep(20000);
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
	draw_scene[0][FOREGROUND] = draw_mandelbrot;
	draw_scene[0][BACKGROUND] = draw_white;
	//
	draw_scene[1][FOREGROUND] = draw_mandelbrot;
	draw_scene[1][BACKGROUND] = draw_cells;
	//
	draw_scene[2][FOREGROUND] = NULL;
	draw_scene[2][BACKGROUND] = draw_fern;
	//
	draw_scene[3][FOREGROUND] = draw_hilbert;
	draw_scene[3][BACKGROUND] = draw_fern;
}

void	init_mbrot_and_white(t_scene *scene, int n)
{
	//MANDEL
	scene->zoom.is_static[n][FOREGROUND] = NO;
	scene->zoom.value[n][FOREGROUND] = 0.001;
	scene->zoom.factor[n][FOREGROUND] = 0.001;
	scene->zoom.exponential[n][FOREGROUND] =  0.0002;
	scene->zoom.speed_up_one[n][FOREGROUND] = 2;
	scene->zoom.stop[n][FOREGROUND] = 75;
	//WHITE
	scene->zoom.is_static[n][BACKGROUND] = YES;
	scene->zoom.value[n][BACKGROUND] = 0;
	scene->zoom.factor[n][BACKGROUND] = 0;
	scene->zoom.exponential[n][BACKGROUND] =  0;
	scene->zoom.speed_up_one[n][BACKGROUND] = 0;
	scene->zoom.stop[n][BACKGROUND] = 0;
	//
	scene->end_determined_by[n] = FOREGROUND;
}

void	init_mbrot_and_cell(t_scene *scene, int n)
{
	//MANDEL
	scene->zoom.is_static[n][FOREGROUND] = NO;
	scene->zoom.value[n][FOREGROUND] = 75;
	scene->zoom.factor[n][FOREGROUND] = 0.01;
	scene->zoom.exponential[n][FOREGROUND] =  0.01;
	scene->zoom.speed_up_one[n][FOREGROUND] = 75;
	scene->zoom.stop[n][FOREGROUND] = 90;
	//CELL
	scene->zoom.is_static[n][BACKGROUND] = NO;
	scene->zoom.value[n][BACKGROUND] = 10.2;
	scene->zoom.factor[n][BACKGROUND] = -0.04;
	scene->zoom.exponential[n][BACKGROUND] = 0;
	scene->zoom.speed_up_one[n][BACKGROUND] = -42;
	scene->zoom.stop[n][BACKGROUND] = 0;
	//
	scene->end_determined_by[n] = BACKGROUND;
}

void	init_fern(t_scene *scene, int n)
{
	//FOREGROUND IS NULL
	//FERN
	scene->zoom.is_static[n][BACKGROUND] = NO;
	scene->zoom.value[n][BACKGROUND] = 0;
	scene->zoom.factor[n][BACKGROUND] = 0.00001;
	scene->zoom.exponential[n][BACKGROUND] = 0.000001;
	scene->zoom.speed_up_one[n][BACKGROUND] = 1;
	scene->zoom.stop[n][BACKGROUND] = 50;
	//
	scene->end_determined_by[n] = BACKGROUND;
}

void	init_fern_and_hilbert(t_scene *scene, int n)
{
	//HILBERT
	scene->zoom.is_static[n][FOREGROUND] = NO;
	scene->zoom.value[n][FOREGROUND] = 0;
	scene->zoom.factor[n][FOREGROUND] = 0.0001;
	scene->zoom.exponential[n][FOREGROUND] =  0.0001;
	scene->zoom.speed_up_one[n][FOREGROUND] = 1;
	scene->zoom.stop[n][FOREGROUND] = 3;
	//FERN
	scene->zoom.is_static[n][BACKGROUND] = NO;
	scene->zoom.value[n][BACKGROUND] = 50;
	scene->zoom.factor[n][BACKGROUND] = 0.01;
	scene->zoom.exponential[n][BACKGROUND] = 0.01;
	scene->zoom.speed_up_one[n][BACKGROUND] = 200;
	scene->zoom.stop[n][BACKGROUND] = 0;
	//
	scene->end_determined_by[n] = FOREGROUND;

}
void	initialize_zoom_values(t_scene *scene)
{
	init_mbrot_and_white(scene, 0);
	init_mbrot_and_cell(scene, 1);
	init_fern(scene, 2);
	init_fern_and_hilbert(scene, 3);
	//init_hilbert(scene, 4);
	//init_mbulb(scene, 5);
}

int	has_scene_ended(t_scene *scene, int n)
{
	int	layer;

	layer = scene->end_determined_by[n];
	if (scene->zoom.value[n][layer] >= scene->zoom.stop[n][layer]
			&& scene->zoom.factor[n][layer] > 0)
		return (YES);
	if (scene->zoom.value[n][layer] <= scene->zoom.stop[n][layer]
			&& scene->zoom.factor[n][layer] < 0)
		return (YES);
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
		scene->zoom.factor[n][layer] += (double)(scene->zoom.exponential[n][layer] * 20);
	else if (scene->zoom.value[n][layer] < scene->zoom.speed_up_one[n][layer]
			&& scene->zoom.factor[n][layer] < 0)
		scene->zoom.factor[n][layer] += (double)(scene->zoom.exponential[n][layer] * 20);
	else
		scene->zoom.factor[n][layer] += scene->zoom.exponential[n][layer];
	scene->zoom.value[n][layer] += scene->zoom.factor[n][layer];
	if (scene->zoom.value[n][layer] > scene->zoom.stop[n][layer]
			&& scene->zoom.factor[n][layer] > 0)
		return (-1);
	if (scene->zoom.value[n][layer] < scene->zoom.stop[n][layer]
			&& scene->zoom.factor[n][layer] < 0)
		return (-1);
	return (scene->zoom.value[n][layer]);
}

void	pre_render_frames_single_scene(t_mlx *mlx, t_scene *scene)
{
	int	scene_num;

	initialize_zoom_values(scene);
	initialize_function_array(scene->draw_scene);
	scene_num = 0;
	while (scene_num < TOTAL_SCENES) //2 scenes max is 1
	{
		new_frame(mlx);
		mlx->zoom = get_zoom_value(scene, scene_num, BACKGROUND);
		if (mlx->zoom >= 0)
			scene->draw_scene[scene_num][BACKGROUND](mlx);
		mlx->zoom = get_zoom_value(scene, scene_num, FOREGROUND);
		if (mlx->zoom >= 0)
			scene->draw_scene[scene_num][FOREGROUND](mlx);
		if (has_scene_ended(scene, scene_num) == YES)
			scene_num++;
	}
	mlx->frame = mlx->head;
}

int	main(void)
{
	t_mlx	mlx;
	t_scene	scene;


	initialize_mlx(&mlx);
	pre_render_frames_single_scene(&mlx, &scene);
	printf("\nFrames rendered!\nPress enter to play and space/tab to pause :)\n");
	//display_menu(&mlx, "./image2.xpm");NOT WORKING BRUUUU
	mlx_hook(mlx.window, KeyPress, KeyPressMask, key_press, &mlx);
	mlx_loop(mlx.mlx);
	//add events to restart, stop, play
	//free list and mlx each node before exit
	return (0);
}
