#ifndef PERLIN_NOISE_H
#define PERLIN_NOISE_H

typedef struct {
	float x;
	float y;
} Vector2;

typedef struct {
	Vector2 **grid;
	unsigned int x;
	unsigned int y;
} PerlinGrid;

PerlinGrid *generate_perlin_grid(unsigned int x, unsigned int y);
float noise(PerlinGrid *perlinGrid, float x, float y);
void print_perlin_grid(PerlinGrid *perlinGrid, unsigned int depth);
void mul_perlin_grid_vector(PerlinGrid *perlinGrid, float len_mul);
void twist_perlin_grid(PerlinGrid *perlinGrid, float angle);

#endif
