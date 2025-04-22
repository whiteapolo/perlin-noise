#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include "perlin_noise.h"
#include <SDL2/SDL.h>

#define WIDTH 12
#define HEIGHT 8
#define DEPTH 100

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

#define FPS 60

Uint32 noise_to_color(float noise)
{
	// grayscale
	unsigned char c = (noise + 1) / 2 * 255;
	return (c << 16) | (c << 8) | c;
}

void render_perlin_noise_tile(PerlinGrid *perlinGrid, Uint32 *pixels, unsigned int x, unsigned int y, unsigned int depth)
{
	for (int y0 = 0; y0 < depth; y0++) {
		for (int x0 = 0; x0 < depth; x0++) {
			Uint32 color = noise_to_color(noise(perlinGrid, x + ((float)x0 / depth), y + ((float)y0 / depth)));
			pixels[(y * depth + y0) * WIDTH * DEPTH + (x * depth + x0)] = color;
		}
	}
}

void render_perlin_noise(PerlinGrid *perlinGrid, Uint32 *pixels, unsigned int depth)
{
	for (int y = 0; y < perlinGrid->y - 1; y++) {
		for (int x = 0; x < perlinGrid->x - 1; x++) {
			render_perlin_noise_tile(perlinGrid, pixels, x, y, depth);
		}
	}
}

int main(void)
{
	srand(time(NULL));

	PerlinGrid *perlinGrid = generate_perlin_grid(WIDTH, HEIGHT);

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *window = SDL_CreateWindow(
			"Perlin Noise",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			WINDOW_WIDTH, WINDOW_HEIGHT,
			SDL_WINDOW_RESIZABLE
	);

    SDL_Renderer *renderer = SDL_CreateRenderer(window,
			-1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	SDL_Texture *texture = SDL_CreateTexture(
			renderer,
			SDL_PIXELFORMAT_RGB888,
			SDL_TEXTUREACCESS_STREAMING,
			WIDTH * DEPTH,
			HEIGHT * DEPTH
	);

	Uint32 *pixels = malloc(sizeof(Uint32) * WIDTH * DEPTH * HEIGHT * DEPTH);

	SDL_Event event;
	bool running = true;
	bool is_expanding = true;
	int iterations = 1;

	while (running) {

		if (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					running = false;
					break;

				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
						case SDLK_ESCAPE:
						case SDLK_q:
							running = false;
							break;
					}
					break;

				case SDL_WINDOWEVENT: {
						switch (event.window.event) {
							case SDL_WINDOWEVENT_RESIZED:
							case SDL_WINDOWEVENT_EXPOSED:
						}
				} break;
			}
		}

		SDL_Delay(1000 / FPS);

		SDL_SetRenderDrawColor(renderer, 0x28, 0x28, 0x28, 0xff);
		SDL_RenderClear(renderer);

		render_perlin_noise(perlinGrid, pixels, DEPTH);
		SDL_UpdateTexture(texture, NULL, pixels, sizeof(Uint32) * WIDTH * DEPTH);

		int window_width;
		int window_height;
		SDL_GetWindowSize(window, &window_width, &window_height);

		SDL_Rect texture_dimentions = {
			.x = (window_width - (WIDTH * DEPTH)) / 2,
			.y = (window_height - (HEIGHT * DEPTH)) / 2,
			.w = WIDTH * DEPTH,
			.h = HEIGHT * DEPTH,
		};

		SDL_RenderCopy(renderer, texture, NULL, &texture_dimentions);

		if (iterations % 250 == 0) {
			is_expanding = !is_expanding;
		}

		// mul_perlin_grid_vector(perlinGrid, is_expanding ? 1.01 : 0.99);
		twist_perlin_grid(perlinGrid, 0.1);

		SDL_RenderPresent(renderer);
		iterations++;
	}

	SDL_DestroyTexture(texture);
	free(pixels);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

	return 0;
}
