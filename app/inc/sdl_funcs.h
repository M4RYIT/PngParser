#pragma once

#include <SDL.h>

int sdl_open_file(const char *file_name, unsigned char **file_data);

int sdl_png_to_static_texture(unsigned char* file_data, unsigned char **pixels, int *w, int *h, int *ch, SDL_Renderer *rnd, SDL_Texture **tex);

int sdl_png_to_streaming_texture(unsigned char* file_data, unsigned char **pixels, int *w, int *h, int *ch, SDL_Renderer *rnd, SDL_Texture **tex);