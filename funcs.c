#include "funcs.h"

Result *find_overlaps(Picture picture, Object object, int *num_results, double threshold)
{
    int picture_dim = (int)sqrt(picture.size);
    int object_dim = (int)sqrt(object.size);
    int max_i = picture_dim - object_dim + 1;
    int max_j = picture_dim - object_dim + 1;

    int results_capacity = max_i * max_j;
    *num_results = 0;
    Result *results = (Result *)malloc(results_capacity * sizeof(Result));

#pragma omp parallel for
    for (int i = 0; i < max_i; i++)
    {
#pragma omp parallel for
        for (int j = 0; j < max_j; j++)
        {
            double sum = 0.0;
            for (int p_i = 0; p_i < object_dim; p_i++)
            {
                for (int p_j = 0; p_j < object_dim; p_j++)
                {
                    int pic_idx = (i + p_i) * picture_dim + (j + p_j);
                    int obj_idx = p_i * object_dim + p_j;
                    double diff = fabs((double)(picture.picture[pic_idx] - object.object[obj_idx]));
                    sum += diff / (double)picture.picture[pic_idx];
                }
            }

            if (sum < threshold)
            {
#pragma omp critical
                {
                    if (*num_results < results_capacity)
                    {
                        Result res;
                        res.i = i;
                        res.j = j;
                        res.picture = picture.id;
                        res.object = object.id;
                        res.found = true;

                        results[*num_results] = res;
                        (*num_results)++;
                    }
                }
            }
        }
    }

    return results;
}