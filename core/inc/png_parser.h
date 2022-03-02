#pragma once

#include "vector.h"

#define ENDIAN_SWITCH_4BYTES(value) ((0x000000ff & value) << 24) | ((0x0000ff00 & value) << 8) | \
                                    ((0x00ff0000 & value) >> 8) | ((0xff000000 & value) >> 24) 

#define RESTRICTIONS 0x00000006

const static char* png_signature = "\x89PNG";
const static int png_len = 4;
const static int png_channels = 4;
const static int png_depth = 8;

typedef struct chunk
{
    char *type;
    int len;
    unsigned char *data;
}
chunk;

int load_png(unsigned char* file_data, unsigned char **pixels, int *w, int *h, int *channels);

int read_chunk(unsigned char **head, chunk *c);

void read_ihdr_ex(chunk *ihdr, int *w, int *h, char *d, char *color_t, char *comp_m, char *filter_m, char *inter_m);

chunk *chunk_by_type(vector *chunks, char *type);

unsigned char paeth(unsigned char a, unsigned char b, unsigned char c);

unsigned char previous(unsigned char *pixels, int x, int y, int row_size);

unsigned char up(unsigned char *pixels, int x, int y, int row_size);

unsigned char up_previous(unsigned char *pixels, int x, int y, int row_size);

unsigned char filter_pixel(unsigned char filter_t, unsigned char px, unsigned char *pixels, int x, int y, int row_size);

void read_ihdr(chunk *ihdr, int *w, int *h, char *d, int* ops);

unsigned char *uncompress_idat_data(int *w, int *h, chunk *idat);

int exit_clear(vector *v, unsigned char *data, int exit_res);