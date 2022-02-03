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

    if (!load_png(file_data, &pixels, &w, &h, &channels)) goto quit;

    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, w, h);
    SDL_UpdateTexture(texture, NULL, pixels, w * channels);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    SDL_Rect rect = {128, 128, 128, 128};

    for (;;)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) goto quit;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, texture, NULL, &rect);

        SDL_RenderPresent(renderer);
    }

quit:
    if (window) SDL_DestroyWindow(window);
    if (renderer) SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}