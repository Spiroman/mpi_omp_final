#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

#define ROOT 0

typdef struct
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

#endif
