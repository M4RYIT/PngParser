#include <string.h>
#include <stdlib.h>
#include <zlib.h>
#include "png_parser.h"

int load_png(unsigned char* file_data, unsigned char **pixels, int *w, int *h, int *channels)
{
    if (strncmp((char*)file_data, png_signature, png_len)) return 0;
    
    unsigned char *full_data = NULL;

    vector *v = vector_new(sizeof(chunk));    
    if (!v) return exit_clear(v, full_data, 0);

    unsigned char* head = file_data+8;   
    chunk c;

    for (;;)
    {
        if (!read_chunk(&head, &c)) return exit_clear(v, full_data, 0);
        vector_append(v, &c);
        if (!strncmp(c.type, "IEND", 4)) break;
    }

    char d; int ops;    

    read_ihdr(vector_at(v,0), w, h, &d, &ops);
    *channels = png_channels;

    if (((ops & RESTRICTIONS) != RESTRICTIONS) || (d != png_depth)) return exit_clear(v, full_data, 0);

    chunk *idat = chunk_by_type(v, "IDAT");
    if (!idat) return exit_clear(v, full_data, 0);

    int row_size = (*w) * png_channels;
    unsigned long size = (*h) * (1 + row_size);

    full_data = uncompress_idat_data(w, h, idat);
    if (!full_data) return exit_clear(v, full_data, 0);

    *pixels = malloc((*h)*row_size);
    if (!*pixels) return exit_clear(v, full_data, 0);

    int i=0; unsigned char f_t;
    for (int y=0; y<(*h); y++)
    {
        f_t = full_data[i];
        i++;

        for (int x=0; x<row_size; x++, i++)
        {
            (*pixels)[x+y*row_size] = (filter_pixel(f_t, full_data[i], *pixels, x, y, row_size) & 0xff);
        }
    }

    return exit_clear(v, full_data, 1);
}

int exit_clear(vector *v, unsigned char *data, int exit_res)
{
    if (v) vector_free(v);
    if (data) free(data);
    return exit_res;
}

unsigned char *uncompress_idat_data(int *w, int *h, chunk *idat)
{
    unsigned long sz = (*h) * (1 + (*w) * png_channels);

    unsigned char *full_data = malloc(sz);
    if (!full_data) return NULL;

    if (uncompress(full_data, &sz, idat->data, (unsigned long)idat->len) != Z_OK)
    {
        free(full_data);
        full_data = NULL;
    }

    return full_data;
}

int read_chunk(unsigned char **head, chunk *c)
{
    c->len = ENDIAN_SWITCH_4BYTES(*((int*)(*head)));
    (*head) += 4;
    
    c->type = (char*)*head;
    (*head) += 4;

    c->data = *head;
    (*head) += c->len;
    
    unsigned int check = crc32(crc32(0, (unsigned char *)c->type, 4), (unsigned char *)c->data, c->len);
    unsigned int expected = ENDIAN_SWITCH_4BYTES(*((int*)(*head)));    
    (*head) += 4;

    return (check!=expected)?0:1;
}

void read_ihdr(chunk *ihdr, int *w, int *h, char *d, int* ops)
{
    unsigned char *ihdr_data = ihdr->data;
    *w = ENDIAN_SWITCH_4BYTES(*((int*)(ihdr_data)));
    ihdr_data += 4;
    
    *h = ENDIAN_SWITCH_4BYTES(*((int*)(ihdr_data)));
    ihdr_data += 4;
    
    *d = *ihdr_data;
    ihdr_data++;

    *ops = *((int*)ihdr_data);
}

void read_ihdr_ex(chunk *ihdr, int *w, int *h, char *d, char *color_t, char *comp_m, char *filter_m, char *inter_m)
{
    unsigned char *ihdr_data = ihdr->data;
    *w = ENDIAN_SWITCH_4BYTES(*((int*)(ihdr_data)));
    ihdr_data += 4;
    
    *h = ENDIAN_SWITCH_4BYTES(*((int*)(ihdr_data)));
    ihdr_data += 4;
    
    *d = *ihdr_data;
    ihdr_data++;
    
    *color_t = *ihdr_data;
    ihdr_data++;
    
    *comp_m = *ihdr_data;
    ihdr_data++;
    
    *filter_m = *ihdr_data;
    ihdr_data++;
    
    *inter_m = *ihdr_data;
}

chunk *chunk_by_type(vector *chunks, char *type)
{
    chunk *c = NULL;

    for (int i=0; i<chunks->count; i++)
    {
        c = vector_at(chunks, i);
        if (!strncmp(c->type, type, 4)) break;
    }

    return c;
}

unsigned char paeth(unsigned char a, unsigned char b, unsigned char c)
{
    int p = (char)a + (char)b - (char)c;
    int pa = abs(p-(char)a);
    int pb = abs(p-(char)b);
    int pc = abs(p-(char)c);

    if (pa <= pb && pa <= pc) return a;

    if (pb <= pc) return b;

    return c;
}

unsigned char previous(unsigned char *pixels, int x, int y, int row_size)
{
    if (x < png_channels) return 0;

    return pixels[y * row_size + x - png_channels];
}

unsigned char up(unsigned char *pixels, int x, int y, int row_size)
{
    if (y <= 0) return 0;

    return pixels[(y-1) * row_size + x];
}

unsigned char up_previous(unsigned char *pixels, int x, int y, int row_size)
{
    if (x < png_channels || y <= 0) return 0;

    return pixels[(y-1) * row_size + x - png_channels];
}

unsigned char filter_pixel(unsigned char filter_t, unsigned char px, unsigned char *pixels, int x, int y, int row_size)
{
    switch(filter_t)
    {
        case 0:
            return px;

        case 1:
            return px + previous(pixels, x, y, row_size);

        case 2:
            return px + up(pixels, x, y, row_size);

        case 3:
            return px + (previous(pixels, x, y, row_size) + up(pixels, x, y, row_size))/2;

        case 4:
            return px + paeth(previous(pixels, x, y, row_size), up(pixels, x, y, row_size), 
                                up_previous(pixels, x, y, row_size));
        
        default:
            return 0;
    }
}