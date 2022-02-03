#include <string.h>
#include "sdl_funcs.h"
#include "png_parser.h"

int sdl_open_file(const char *file_name, unsigned char **file_data)
{
    SDL_RWops *file = SDL_RWFromFile(file_name, "rb");
    if (!file) return 0;

    long long file_size = SDL_RWsize(file);

    *file_data = malloc(file_size);
    if (!*file_data)
    {
        SDL_RWclose(file);
        return 0;
    }

    SDL_RWread(file, *file_data, file_size, 1);
    SDL_RWclose(file);
    return 1;
}

int sdl_png_to_static_texture(unsigned char* file_data, unsigned char **pixels, int *w, int *h, int *ch, SDL_Renderer *rnd, SDL_Texture **tex)
{
    if (!load_png(file_data, pixels, w, h, ch)) return 0;

    *tex = SDL_CreateTexture(rnd, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, *w, *h);
    if (!*tex) return 0;

    SDL_UpdateTexture(*tex, NULL, *pixels, (*w)*(*ch));
    SDL_SetTextureBlendMode(*tex, SDL_BLENDMODE_BLEND);

    free(*pixels);
    return 1;
}

int sdl_png_to_streaming_texture(unsigned char* file_data, unsigned char **pixels, int *w, int *h, int *ch, SDL_Renderer *rnd, SDL_Texture **tex)
{
    unsigned char *temp;
    if (!load_png(file_data, &temp, w, h, ch)) return 0;

    *tex = SDL_CreateTexture(rnd, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, *w, *h);
    if (!*tex) return 0;

    int pitch;
    SDL_LockTexture(*tex, NULL, (void**)pixels, &pitch);

    int row_size = (*w)*(*ch);
    for (int y=0; y<(*h); y++)
    {
        memcpy((*pixels)+y*pitch, 
                    temp+y*row_size,
                    row_size);
    }

    SDL_UnlockTexture(*tex);
    SDL_SetTextureBlendMode(*tex, SDL_BLENDMODE_BLEND);
    free(temp);

    return 1;
}