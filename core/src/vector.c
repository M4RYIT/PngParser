#include <stdlib.h>
#include <string.h>
#include "vector.h"

vector *vector_new(size_t t)
{
    vector *v = malloc(sizeof(vector));
    if (!v) return NULL;

    v->type = t;
    v->count = 0;
    v->items = NULL;
    return v;
}

void vector_free(vector *v)
{
    v->count = 0;
    free(v->items);
    v->items = NULL;
    free(v);
}

void vector_append(vector *v, void *value)
{
    v->items = realloc(v->items, v->type * (v->count+1));    
    memcpy(v->items + (v->count * v->type), value, v->type);
    v->count++;
}

void *vector_at(vector *v, int index)
{
    if (index < 0 || index >= v->count) return NULL;

    return v->items + (index * v->type);
}

void *vector_last(vector *v)
{
    return vector_at(v, v->count-1);
}