#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "mpi.h"
#include "data.h"

void create_mpi_result_type(MPI_Datatype *mpi_result_type);
void create_mpi_object_type(MPI_Datatype *mpi_object_type);
void create_mpi_picture_type(MPI_Datatype *mpi_picture_type);

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int world_rank, world_size;

    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Declare MPI datatypes
    MPI_Datatype MPI_Picture, MPI_Object, MPI_Result;

    // Create MPI datatypes
    create_mpi_picture_type(&MPI_Picture);
    create_mpi_object_type(&MPI_Object);
    create_mpi_result_type(&MPI_Result);

    // Only the root process reads the input file
    if (world_rank == 0)
    {
        float matching;
        int picture_size, object_size, num_pictures, num_objects;

        // Read matching score from stdin
        scanf("%f", &matching);

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

        // Send picture and object data to worker threads
        int worker_rank = 1;
        for (int i = 0; i < num_pictures; i++)
        {
            for (int j = 0; j < num_objects; j++)
            {
                MPI_Send(&picture_array[i], 1, MPI_Picture, worker_rank, 0, MPI_COMM_WORLD);
                MPI_Send(picture_array[i].picture, picture_array[i].size, MPI_INT, worker_rank, 1, MPI_COMM_WORLD);
                MPI_Send(&object_array[j], 1, MPI_Object, worker_rank, 0, MPI_COMM_WORLD);
                MPI_Send(object_array[j].object, object_array[j].size, MPI_INT, worker_rank, 1, MPI_COMM_WORLD);
                worker_rank = (worker_rank + 1) % world_size;
            }
        }
        // Free memory allocated for the picture and object arrays
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
    else
    {
        // Worker threads receive picture and object data from root process
        Picture picture;
        Object object;
        MPI_Recv(&picture, 1, MPI_Picture, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        picture.picture = (int *)malloc(sizeof(int) * picture.size);
        MPI_Recv(picture.picture, picture.size, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&object, 1, MPI_Object, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        object.object = (int *)malloc(sizeof(int) * object.size);
        MPI_Recv(object.object, object.size, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Print the received picture and object
        printf("Worker %d received picture with id %d and object with id %d:\n", world_rank, picture.id, object.id);
        printf("picture size %d received picture with id %d and object size %d with id %d:\n", picture.size, picture.id, object.size, object.id);
        printf("Picture:\n");
        for (int i = 0; i < picture.size; i++)
        {
            printf("%d ", picture.picture[i]);
            if ((i + 1) % (int)sqrt(picture.size) == 0)
            {
                printf("\n");
            }
        }
        printf("Object:\n");
        for (int i = 0; i < object.size; i++)
        {
            printf("%d ", object.object[i]);
            if ((i + 1) % (int)sqrt(object.size) == 0)
            {
                printf("\n");
            }
        }
    }

    // Finalize the MPI environment.
    MPI_Type_free(&MPI_Picture);
    MPI_Type_free(&MPI_Object);
    MPI_Type_free(&MPI_Result);

    MPI_Finalize();
    return 0;
}

void create_mpi_picture_type(MPI_Datatype *mpi_picture_type)
{
    Picture p;
    int blocklengths[2] = {1, 1};
    MPI_Aint displacements[2];
    MPI_Datatype types[2] = {MPI_INT, MPI_INT};

    MPI_Aint intex, intey;
    MPI_Get_address(&p.id, &intex);
    MPI_Get_address(&p.size, &intey);

    displacements[0] = intex - intex;
    displacements[1] = intey - intex;

    MPI_Type_create_struct(2, blocklengths, displacements, types, mpi_picture_type);
    MPI_Type_commit(mpi_picture_type);
}

void create_mpi_object_type(MPI_Datatype *mpi_object_type)
{
    Object o;
    int blocklengths[2] = {1, 1};
    MPI_Aint displacements[2];
    MPI_Datatype types[2] = {MPI_INT, MPI_INT};

    MPI_Aint intex, intey;
    MPI_Get_address(&o.id, &intex);
    MPI_Get_address(&o.size, &intey);

    displacements[0] = intex - intex;
    displacements[1] = intey - intex;

    MPI_Type_create_struct(2, blocklengths, displacements, types, mpi_object_type);
    MPI_Type_commit(mpi_object_type);
}

void create_mpi_result_type(MPI_Datatype *mpi_result_type)
{
    Result r;
    int blocklengths[5] = {1, 1, 1, 1, 1};
    MPI_Aint displacements[5];
    MPI_Datatype types[5] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_C_BOOL};

    MPI_Aint intex, intey, intez, intew, inteb;
    MPI_Get_address(&r.i, &intex);
    MPI_Get_address(&r.j, &intey);
    MPI_Get_address(&r.picture, &intez);
    MPI_Get_address(&r.object, &intew);
    MPI_Get_address(&r.found, &inteb);

    displacements[0] = intex - intex;
    displacements[1] = intey - intex;
    displacements[2] = intez - intex;
    displacements[3] = intew - intex;
    displacements[4] = inteb - intex;

    MPI_Type_create_struct(5, blocklengths, displacements, types, mpi_result_type);
    MPI_Type_commit(mpi_result_type);
}
