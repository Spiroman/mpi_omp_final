#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

#include <stdbool.h>
#include "uthash.h"
#define ROOT 0
#define STOP_WORKER 3

typedef struct
{
    int id;
    int size;
    int *picture;
} Picture;

typedef struct
{
    int id;
    int size;
    int *object;
} Object;

typedef struct
{
    int i;
    int j;
    int picture;
    int object;
    bool found;
} Result;

typedef struct
{
    int picture_id;
    int count;
    Result *results;
    UT_hash_handle hh;
} PictureResult;

#endif
