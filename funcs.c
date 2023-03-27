#include <math.h>
#include <stdbool.h>
#include <omp.h>
#include "data.h"
#include <stdio.h>

void find_overlap(Picture picture, Object object, double threshold, Result *result)
{
    result->picture = picture.id;
    result->object = object.id;
    result->found = false;

    int picture_dim = (int)sqrt(picture.size);
    int object_dim = (int)sqrt(object.size);
    int max_i = picture_dim - object_dim + 1;
    int max_j = picture_dim - object_dim + 1;

    // Collapse indicates that the two loops are run in parallel
#pragma omp parallel for collapse(2)
    for (int i = 0; i < max_i; i++)
    {
        for (int j = 0; j < max_j; j++)
        {
            float sum = 0.0;
            for (int o_i = 0; o_i < object_dim; o_i++)
            {
                for (int o_j = 0; o_j < object_dim; o_j++)
                {
                    int pic_idx = (i + o_i) * picture_dim + (j + o_j);
                    int obj_idx = o_i * object_dim + o_j;
                    float diff = (float)abs((picture.picture[pic_idx] - object.object[obj_idx]));
                    sum += diff / (float)picture.picture[pic_idx];
                }
            }
            // Make sure that we get a correct result, and that only one thread updates the result
#pragma omp critical
            {
                if (sum < threshold && !result->found)
                {
                    result->i = i;
                    result->j = j;
                    result->found = true;
                }
            }
        }
    }
}
