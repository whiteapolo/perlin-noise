#include "perlin_noise.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <sys/types.h>

float random_float(float min, float max)
{
	return (((float)rand()) / RAND_MAX) * 2.0 - 1.0;
}

void **mat_alloc(unsigned int x, unsigned int y, size_t element_size)
{
	void **mat = malloc(sizeof(void *) * y);

	for (int i = 0; i < y; i++) {
		mat[i] = malloc(element_size * x);
	}

	return mat;
}

Vector2 random_vector2(float desired_len)
{
	float x = random_float(-1, 1);
	float y = random_float(-1, 1);
	float len = sqrtf(x * x + y * y);

	if (len == 0.0f) {
		return (Vector2) {
			.x = desired_len,
			.y = 0,
		};
	}

	return (Vector2) {
		.x = x * desired_len / len,
		.y = y * desired_len / len,
	};
}

PerlinGrid *generate_perlin_grid(unsigned int x, unsigned int y)
{
	PerlinGrid *perlinGrid = malloc(sizeof(PerlinGrid));

	perlinGrid->y = y + 1;
	perlinGrid->x = x + 1;
	perlinGrid->grid = (Vector2 **)mat_alloc(perlinGrid->x, perlinGrid->y, sizeof(Vector2));

	for (int i = 0; i < perlinGrid->y; i++) {
		for (int j = 0; j < perlinGrid->x; j++) {
			perlinGrid->grid[i][j] = random_vector2(1);
		}
	}

	return perlinGrid;
}

float fade(float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float dot_product(float x1, float y1, float x2, float y2)
{
	return x1 * x2 + y1 * y2;
}

float noise(PerlinGrid *perlinGrid, float x, float y)
{
	unsigned int x0 = (unsigned int)x;
	unsigned int y0 = (unsigned int)y;
	unsigned int x1 = x0 + 1;
	unsigned int y1 = y0 + 1;

	float sx = x - x0;
	float sy = y - y0;
	float u = fade(sx);
	float v = fade(sy);

	float n1 = dot_product(perlinGrid->grid[y0][x0].x, perlinGrid->grid[y0][x0].y, sx, sy);
	float n2 = dot_product(perlinGrid->grid[y0][x1].x, perlinGrid->grid[y0][x1].y, sx - 1, sy);
	float n3 = dot_product(perlinGrid->grid[y1][x0].x, perlinGrid->grid[y1][x0].y, sx, sy - 1);
	float n4 = dot_product(perlinGrid->grid[y1][x1].x, perlinGrid->grid[y1][x1].y, sx - 1, sy - 1);

	// linear interpelation
	float bottom = (1 - u) * n1 + u * n2;
	float top = (1 - u) * n3 + u * n4;
	float result = (1 - v) * bottom + v * top;

	return result;
}

// depth should devide 1 by whole number
void print_perlin_tile(PerlinGrid *perlinGrid, unsigned int x, unsigned int y, unsigned int depth)
{
	for (int y0 = 0; y0 < depth; y0++) {
		for (int x0 = 0; x0 < depth; x0++) {
			printf("%6.1f ", noise(perlinGrid, x + ((float)x0 / depth), y + ((float)y0 / depth)));
		}
		printf("\n");
	}
}

void print_perlin_grid(PerlinGrid *perlinGrid, unsigned int depth)
{
	for (int y = 0; y < perlinGrid->y - 1; y++) {
		for (int x = 0; x < perlinGrid->x - 1; x++) {
			print_perlin_tile(perlinGrid, x, y, depth);
		}
	}
}

Vector2 rotate_vector(Vector2 v, float angle)
{
	float cos_a = cosf(angle);
	float sin_a = sinf(angle);

	Vector2 result = {
		.x = v.x * cos_a - v.y * sin_a,
		.y = v.x * sin_a + v.y * cos_a
	};

	return result;
}

Vector2 mul_vector(Vector2 vec, float t)
{
	Vector2 result = {
		.x = vec.x * t,
		.y = vec.y * t,
	};

	return result;
}

void mul_perlin_grid_vector(PerlinGrid *perlinGrid, float len_mul)
{
	for (int y = 0; y < perlinGrid->y; y++) {
		for (int x = 0; x < perlinGrid->x; x++) {
			perlinGrid->grid[y][x] = mul_vector(perlinGrid->grid[y][x], len_mul);
		}
	}
}

void twist_perlin_grid(PerlinGrid *perlinGrid, float angle)
{
	for (int y = 0; y < perlinGrid->y; y++) {
		for (int x = 0; x < perlinGrid->x; x++) {
			perlinGrid->grid[y][x] = rotate_vector(perlinGrid->grid[y][x], angle);
		}
	}
}
