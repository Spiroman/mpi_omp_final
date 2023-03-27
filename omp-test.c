#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "data.h"
#include "funcs.h"

int main()
{
    float threshold;

    // float matching;
    int picture_size, object_size, num_pictures, num_objects;

    // Read matching score from stdin
    scanf("%f", &threshold);
    // Broadcast matching score to all worker processes

    // Read number of pictures from stdin
    scanf("%d", &num_pictures);

    // Allocate memory for the picture array
    Picture *picture_array = (Picture *)malloc(sizeof(Picture) * num_pictures);

    // Initialize each picture in the array
    for (int i = 0; i < num_pictures; i++)
    {
        // Read id and size of the picture from stdin
        scanf("%d", &picture_array[i].id);
        scanf("%d", &picture_size);

        // Set size of the picture to picture_size^2
        picture_array[i].size = picture_size * picture_size;

        // Allocate memory for the picture array
        picture_array[i].picture = (int *)malloc(sizeof(int) * picture_array[i].size);

        // Initialize each pixel in the picture
        for (int j = 0; j < picture_array[i].size; j++)
        {
            scanf("%d", &picture_array[i].picture[j]);
        }
    }

    // Read number of objects from stdin
    scanf("%d", &num_objects);

    // Allocate memory for the object array
    Object *object_array = (Object *)malloc(sizeof(Object) * num_objects);

    // Initialize each object in the array
    for (int i = 0; i < num_objects; i++)
    {
        // Read id and size of the object from stdin
        scanf("%d", &object_array[i].id);
        scanf("%d", &object_size);

        // Set size of the object to picture_size^2
        object_array[i].size = object_size * object_size;

        // Compute width and height of the object

        // Allocate memory for the object array
        object_array[i].object = (int *)malloc(sizeof(int) * object_array[i].size);

        // Initialize each pixel in the object
        for (int j = 0; j < object_array[i].size; j++)
        {
            scanf("%d", &object_array[i].object[j]);
        }
    }

    printf("Number of pictures: %d, number of objects: %d\n", num_pictures, num_objects);

    for (int i = 0; i < num_pictures; i++)
    {
        for (int j = 0; j < num_objects; j++)
        {
            int num_results;
            Result *results = find_overlaps(picture_array[i], object_array[j], &num_results, threshold);

            // Print the results
            printf("Results for picture %d and object %d:\n", picture_array[i].id, object_array[j].id);
            for (int k = 0; k < num_results; k++)
            {
                if (results[k].found)
                {
                    printf("Result %d: i=%d, j=%d, picture=%d, object=%d\n", k, results[k].i, results[k].j, results[k].picture, results[k].object);
                }
            }

            // Free memory allocated for results
            if (results != NULL)
            {
                free(results);
            }
        }
    }
    for (int i = 0; i < num_pictures; i++)
    {
        free(picture_array[i].picture);
    }
    free(picture_array);

    for (int i = 0; i < num_objects; i++)
    {
        free(object_array[i].object);
    }
    free(object_array);
}
