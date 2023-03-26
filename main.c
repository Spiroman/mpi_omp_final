#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "data.h"
#include "mpi.h"

int main(int argc, char *argv[])
{
    int rank, size;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    float matchingValue;
    int numOfPictures;
    if (rank == ROOT)
    {
        float matching;
        int picture_size, num_pictures, num_objects;

        // Read matching score from stdin
        scanf("%f", &matching);

        // Read number of pictures from stdin
        scanf("%d", &num_pictures);

        // Allocate memory for the picture array
        struct Picture *picture_array = (struct Picture *)malloc(sizeof(struct Picture) * num_pictures);

        // Initialize each picture in the array
        for (int i = 0; i < num_pictures; i++)
        {
            int width, height;

            // Read id and size of the picture from stdin
            scanf("%d", &picture_array[i].id);
            scanf("%d", &picture_size);

            // Set size of the picture
            picture_array[i].size = picture_size;

            int real_picture_size = picture_array[i].size * picture_array[i].size;

            // Allocate memory for the picture array
            picture_array[i].picture = (int *)malloc(sizeof(int) * real_picture_size);

            // Initialize each pixel in the picture
            for (int j = 0; j < real_picture_size; j++)
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
            int width, height;

            // Read id and size of the object from stdin
            scanf("%d", &object_array[i].id);
            scanf("%d", &picture_size);

            // Set size of the object to picture_size^2
            object_array[i].size = picture_size;

            int real_object_size = object_array[i].size * object_array[i].size;

            // Allocate memory for the object array
            object_array[i].object = (int *)malloc(sizeof(int) * real_object_size);

            // Initialize each number in the object
            for (int j = 0; j < real_object_size; j++)
            {
                scanf("%d", &object_array[i].object[j]);
            }
        }

        // // Dynamic work allocation - similar structure to the first HW1 assignment.
        // int jobs_sent = 0;
        // for (int worker_id = 1; worker_id < size; worker_id++)
        // {
        //     // Send base sequence length
        //     MPI_Send(baseSeq, baseSeqLen, MPI_CHAR, worker_id, WORK, MPI_COMM_WORLD);
        //     // Send base sequence itself
        //     MPI_Send(cmpSeqs[worker_id - 1], cmpSeqsLengths[worker_id - 1], MPI_CHAR, worker_id, WORK, MPI_COMM_WORLD);
        //     // Increase sent jobs
        //     jobs_sent++;
        // }

        // for (int jobs_done = 0; jobs_done <= numOfcmpSeqs - 1; jobs_done++)
        // {
        //     // Temporary holder for the result from the worker thread
        //     Result res;

        //     MPI_Recv(&res, 1, MPI_RESULT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        //     int jobs_left = numOfcmpSeqs - jobs_sent;
        //     if (jobs_left > 0)
        //     {

        //         MPI_Send();
        //         jobs_sent++;
        //     }
        //     else
        //     {
        //         /* send STOP message. message has no data */
        //         char dummy;
        //         MPI_Send(&dummy, 0, MPI_CHAR, status.MPI_SOURCE, STOP, MPI_COMM_WORLD);
        //     }

        // }

        // Free memory allocated for the picture array
        for (int i = 0; i < num_pictures; i++)
        {
            free(picture_array[i].picture);
        }
        free(picture_array);

        // Free memory allocated for the object array
        for (int i = 0; i < num_objects; i++)
        {
            free(object_array[i].object);
        }
        free(object_array);

        else
        {
        }
    }