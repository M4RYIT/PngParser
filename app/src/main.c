#include <SDL.h>
#include "sdl_funcs.h"
#include "png_parser.h"

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("BMP Parser", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 512, 512, 0);
    if (!window) goto quit;

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) goto quit;

    char *file_name = "assets\\img.png";
    unsigned char *file_data;
    unsigned char *pixels;
    int w, h, channels;

    if (!sdl_open_file(file_name, &file_data)) goto quit;

    SDL_Texture *static_texture;
    SDL_Texture *streaming_texture;

    if (!sdl_png_to_static_texture(file_data, &pixels, &w, &h, &channels, renderer, &static_texture)) goto quit;

    if (!sdl_png_to_streaming_texture(file_data, &pixels, &w, &h, &channels, renderer, &streaming_texture)) goto quit;

    SDL_Rect rect = {64, 64, 128, 128};

    for (;;)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) goto quit;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, static_texture, NULL, &rect);

        rect.x = 320;
        SDL_RenderCopy(renderer, streaming_texture, NULL, &rect);

        rect.x = 64;
        SDL_RenderPresent(renderer);
    }

quit:
    if (window) SDL_DestroyWindow(window);
    if (renderer) SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}