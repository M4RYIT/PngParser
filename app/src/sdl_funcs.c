#include <SDL.h>
#include "sdl_funcs.h"

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