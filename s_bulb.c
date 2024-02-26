#include "fractal.h"
#include <complex.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <immintrin.h> // Include for SIMD

#define TOTAL_THREADS 12
#include <immintrin.h> // Include for SIMD

static double spherical_radius(t_coordinates *z, t_coordinates c, int n) {
    // Load z coordinates into SIMD registers
    __m128d z_x = _mm_set1_pd(z->x);
    __m128d z_y = _mm_set1_pd(z->y);
    __m128d z_z = _mm_set1_pd(z->z);

    // Compute z.r = sqrt(z.x^2 + z.y^2 + z.z^2)
    __m128d z_x2 = _mm_mul_pd(z_x, z_x);
    __m128d z_y2 = _mm_mul_pd(z_y, z_y);
    __m128d z_z2 = _mm_mul_pd(z_z, z_z);
    __m128d z_r = _mm_sqrt_pd(_mm_add_pd(_mm_add_pd(z_x2, z_y2), z_z2));

    // Compute z.theta = atan2(sqrt(z.x^2 + z.y^2), z.z)
    __m128d z_theta = 0;
	z_theta = _mm_atan2_pd(_mm_sqrt_pd(_mm_add_pd(z_x2, z_y2)), z_z);

    // Compute z.phi = atan2(z.y, z.x)
    __m128d z_phi = _mm_atan2_pd(z_y, z_x);

    // Compute power_of(z.r, n)
    __m128d z_r_pow_n = simd_power_of(z_r, _mm_set1_pd(n));

    // Compute cos(z.theta * n), cos(z.phi * n), and tan(z.phi * n)
    __m128d cos_theta_n = _mm_cos_pd(_mm_mul_pd(z_theta, _mm_set1_pd(n)));
    __m128d cos_phi_n = _mm_cos_pd(_mm_mul_pd(z_phi, _mm_set1_pd(n)));
    __m128d tan_phi_n = _mm_tan_pd(_mm_mul_pd(z_phi, _mm_set1_pd(n)));

    // Compute new coordinates using SIMD
    __m128d new_x = _mm_mul_pd(_mm_mul_pd(z_r_pow_n, cos_theta_n), cos_phi_n);
    __m128d new_y = _mm_mul_pd(_mm_mul_pd(z_r_pow_n, _mm_sin_pd(_mm_mul_pd(z_theta, _mm_set1_pd(n)))), tan_phi_n);
    __m128d new_z = _mm_mul_pd(z_r_pow_n, cos_theta_n);

    // Add c.x, c.y, c.z
    new_x = _mm_add_pd(new_x, _mm_set1_pd(c.x));
    new_y = _mm_add_pd(new_y, _mm_set1_pd(c.y));
    new_z = _mm_add_pd(new_z, _mm_set1_pd(c.z));

    // Store the results back to z
    _mm_store_pd(&z->x, new_x);
    _mm_store_pd(&z->y, new_y);
    _mm_store_pd(&z->z, new_z);

    // Calculate and return z.r
    double result[2];
    _mm_store_pd(result, z_r);
    return result[0]; // Assuming z.r is the same for both elements
}

static double	power_of(double base, int exponent)
{
    if (exponent < 0) {
        return 1.0 / power_of(base, -exponent);
    }

    double result = 1.0;
    while (exponent > 0) {
        if (exponent & 1) {
            result *= base;
        }
        base *= base;
        exponent >>= 1;
    }

    return result;
}

static void	ft_mlx_pixel_put(t_data *data, int x, int y, int color)
{
	char	*dst;

	dst = data->address + (y * data->line_length
			+ x * (data->bits_per_pixel / 8));
	*(unsigned int *)dst = color;
}

static double	map(double pixel, double total_pixels, double fractal_min, double fractal_max)
{
	double	result;

	result = fractal_min + ((fractal_max - fractal_min) / (total_pixels - 1) * pixel);
	return (result);
}

static void	rotate_and_draw(double complex x, double complex y, int color, t_mlx *mlx)
{
	x = mlx->width - x;
	if ((int)x < mlx->width && (int)y < mlx->height && (int)x >= 0 && (int)y >= 0)
		ft_mlx_pixel_put(&mlx->frame->image, (int)x, (int)y, color);
	y = mlx->height - 1 - y;
	if ((int)x < mlx->width && (int)y < mlx->height && (int)x >= 0 && (int)y >= 0)
		ft_mlx_pixel_put(&mlx->frame->image, (int)x, (int)y, color);
	x = mlx->width - x;
	if ((int)x < mlx->width && (int)y < mlx->height && (int)x >= 0 && (int)y >= 0)
		ft_mlx_pixel_put(&mlx->frame->image, (int)x, (int)y, color);
}



static void	set_fractal_range(t_mlx *mlx)
{
	mlx->range.x_max = 1 * mlx->zoom;
	mlx->range.y_max = 1 * mlx->zoom;
	mlx->range.x_min = -1 * mlx->zoom;
	mlx->range.y_min = -1 * mlx->zoom;
}


static int	lerp_colors(int	color_start, int color_end, double step)
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
/*
//COLOR 6
int	get_color_palette_shade(double step)
{
	int	color[5];
	int	index;

	if (step < 0.4)
		step *= 1.3; //1.5 is v2 and 1.3 is v3
	color[0] = 0x0680e03;
	color[1] = 0x0b11509;
	color[2] = 0x0fc5e1d;
	color[3] = 0x0b11509;
	color[4] = 0x0680e03;
	index = ceil(step * 5) - 1;
	step = (step * 5) - index;
	return (lerp_colors(color[index], color[index + 1], step));
}
*/

//COLOR 5
static int	get_color_palette_shade(double step)
{
	int	color[5];
	int	index;

	//if (step < 0.4)
	//	step *= 1.3; //1.5 is v2 and 1.3 is v3
	//for pink color5 with shade no 1.3 or 1.5
	color[0] = 0x00b1f3a;
	color[1] = 0x076101e;
	color[2] = 0x0da8a8b;
	color[3] = 0x0c9374c;
	color[4] = 0x0c2dde4;
	index = ceil(step * 5) - 1;
	step = (step * 5) - index;
	return (lerp_colors(color[index], color[index + 1], step));
}

/*
//COLOR 3
int	get_color_palette_shade(double step)
{
	int	color[5];
	int	index;

	//if (step < 0.4)
	//	step *= 1.3; //1.5 is v2 and 1.3 is v3
	color[0] = 0x0240002;
	color[1] = 0x06f0100;
	color[2] = 0x0a53005;
	color[3] = 0x0d97d0c;
	color[4] = 0x0fec135;
	index = ceil(step * 5) - 1;
	step = (step * 5) - index;
	return (lerp_colors(color[index], color[index + 1], step));
}*/
/*
static void	draw_background(t_mlx *mlx, t_coordinates c, t_coordinates pixel)
{
	double	distance_from_center;
	double	max_distance_from_center;
	double	step;
	int	color;

	distance_from_center = sqrt(pow(c.x, 2) + pow(c.y, 2));
	max_distance_from_center = sqrt(pow(mlx->range.x_min, 2) + pow(mlx->range.y_min, 50));
	step = distance_from_center / max_distance_from_center;

	if (step > 1)
		step = 1;
	color = lerp_colors(0x0000435, 0x0000000, step);
	if (pixel.x == 400 && pixel.y == 400)
		printf("color at 400: %d\n", color);
	if (pixel.x == 399 && pixel.y == 399)
		printf("color at 399: %d\n", color);
	if ((int)pixel.x < 800 && (int)pixel.y < 800 && (int)pixel.x >= 0 && (int)pixel.y >= 0)
	ft_mlx_pixel_put(&mlx->frame->image, (int)pixel.x, (int)pixel.y, color);
	//rotate_and_draw(pixel.x, pixel.y, color, mlx);
}*/

static void	draw_mandelbulb(t_mlx *mlx, t_coordinates c, t_coordinates p)
{
	double	step;
	int color;

	step = c.z;
	step /= 1.5; //this is max z
	color = get_color_palette_shade(step);
	ft_mlx_pixel_put(&mlx->frame->image, (int)p.x, (int)p.y, color);
	rotate_and_draw(p.x, p.y, color, mlx);
}

static int	is_mandelbulb(t_coordinates *c, t_mlx *mlx)
{
	t_coordinates	z;
	double	i;
	double	max_c_z;
	z.iterations = 10;

	max_c_z = -1;
	c->z = 0;
	while (c->z < 1.2) //1.1 is max z
	{
		i = -1.0;
		z.x = 0;
		z.y = 0;
		z.z = 0;
		while (++i < z.iterations)
		{
			if (spherical_radius(&z, *c, mlx->n) > 2)
				break;
		}
		if (i == z.iterations)
			max_c_z = c->z;
		c->z += c->factor;
	}
	c->z = max_c_z;
	if (max_c_z == -1)
		return (0);
	if (c->z > mlx->max.z)
		mlx->max.z = c->z;
	return (1);
}

static void	plot_points(t_thread *args)
{
	t_coordinates	pixel;
	t_coordinates	c;

	c.factor = 1;
	c.factor /= (args->mlx->width / (args->mlx->range.x_max - args->mlx->range.x_min));
	pixel.x = args->first;
	while (++pixel.x < args->last)
	{
		c.x = map(pixel.x, args->mlx->width, args->mlx->range.x_min, args->mlx->range.x_max);
		pixel.y = -1;
		while (++pixel.y < args->mlx->height / 2)
		{
			c.y = map(pixel.y, args->mlx->height, args->mlx->range.y_max, args->mlx->range.y_min);
			if (is_mandelbulb(&c, args->mlx))
				draw_mandelbulb(args->mlx, c, pixel);
		}
	}
}

void	draw_bulb(t_mlx *mlx)
{
	int			i;
	pthread_t	thread[TOTAL_THREADS];
	t_thread	args[TOTAL_THREADS];

	set_fractal_range(mlx);
	i = -1;
	while (++i < TOTAL_THREADS)
	{
		args[i].mlx = mlx;
		if (i == 0)
			args[i].first = -1;
		else
		{
			args[i].first = args[i - 1].last - 1;
			args[i].x = args[i - 1].x;
		}
		args[i].last = args[i].first + (mlx->width / 2 / TOTAL_THREADS) + 2;
		if (i == TOTAL_THREADS -1 && args[i].last != mlx->width / 2)
			args[i].last = (mlx->width / 2) + 1;
		pthread_create(&thread[i], NULL, (void *(*)(void *))&plot_points, (void *)&args[i]);
	}
	i = -1;
	while (++i < TOTAL_THREADS)
		pthread_join(thread[i], NULL);
}
