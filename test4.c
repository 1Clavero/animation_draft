 /* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test3.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
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
	mlx->window = mlx_new_window(mlx->mlx, mlx->width, mlx->height, "test2");
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
	draw_scene[0][0] = draw_mandelbrot;
	draw_scene[0][1] = NULL;
	//draw_scene[1] = draw_cells;
}

void	initialize_zoom_values(t_scene	*scene)
{
	//MANDEL
	scene->zoom.value[0][0] = 0.001;
	scene->zoom.factor[0][0] = 0.001;
	scene->zoom.exponential[0][0] =  0.0002;
	scene->zoom.speed_up_one[0][0] = 2;
	scene->zoom.stop[0][0] = 200;
	/*//CELL
	zoom->value[1] = 10.2;
	zoom->factor[1] = -0.001;
	zoom->exponential[1] = 0;
	zoom->max[1] = 0;
	zoom->empty_background[1] = 0.2;
	//GENERAL
	*scene = 0;*/
}

int	has_scene_ended(t_scene *scene, int n)
{
	if (scene->zoom.value[n][BACKGROUND] >= scene->zoom.stop[n][BACKGROUND])
		return (0);
	return (1);
}

double	get_zoom_value(t_scene *scene, int n, int layer)
{
	if (scene->draw_scene[n][layer] == NULL)
		return (-1);
	if (scene->zoom.value[n][layer] > scene->zoom.speed_up_one[n][layer])
		scene->zoom.factor[n][layer] += (double)(scene->zoom.exponential[n][layer] * 20);
	scene->zoom.factor[n][layer] += scene->zoom.exponential[n][layer];
	scene->zoom.value[n][layer] += scene->zoom.factor[n][layer];
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
		scene->draw_scene[scene_num][BACKGROUND](mlx);
		mlx->zoom = get_zoom_value(scene, scene_num, FOREGROUND);
		if ((int)mlx->zoom != -1)
			scene->draw_scene[scene_num][FOREGROUND](mlx);
		if (has_scene_ended(scene, scene_num) == 0)
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
	printf("\nFrames rendered!\n");
	//display_menu(&mlx, "./image2.xpm");NOT WORKING BRUUUU
	mlx_hook(mlx.window, KeyPress, KeyPressMask, key_press, &mlx);
	mlx_loop(mlx.mlx);
	//add events to restart, stop, play
	//free list and mlx each node before exit
	return (0);
}
