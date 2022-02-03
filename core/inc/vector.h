#pragma once

#include <stdint.h>

typedef struct vector
{
    uint8_t *items;
    size_t type;
    int count;
}
vector;

vector *vector_new(size_t t);

void vector_free(vector *v);

void vector_append(vector *v, void *value);

void *vector_at(vector *v, int index);

void *vector_last(vector *v);