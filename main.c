#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "mpi.h"
#include "data.h"
#include "funcs.h"

// Result *find_overlaps(Picture picture, Object object, int *num_results, double threshold);
void create_mpi_result_type(MPI_Datatype *mpi_result_type);
void create_mpi_object_type(MPI_Datatype *mpi_object_type);
void create_mpi_picture_type(MPI_Datatype *mpi_picture_type);

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, world_size;
    float threshold;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Declare MPI datatypes
    MPI_Datatype MPI_Picture, MPI_Object, MPI_Result;

    // Create MPI datatypes
    create_mpi_picture_type(&MPI_Picture);
    create_mpi_object_type(&MPI_Object);
    create_mpi_result_type(&MPI_Result);

    int num_results;
    Result *results;
    Result result;

    // Only the root process reads the input file
    if (rank == 0)
    {
        // float matching;
        int picture_size, object_size, num_pictures, num_objects;

        FILE *inputFile = fopen("input.txt", "r");
        if (inputFile == NULL)
        {
            printf("Error opening the input file.\n");
            return 1;
        }

        // Read matching score from stdin
        fscanf(inputFile, "%f", &threshold);
        // Broadcast matching score to all worker processes
        MPI_Bcast(&threshold, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

        // Read number of pictures from stdin
        fscanf(inputFile, "%d", &num_pictures);

        // Allocate memory for the picture array
        Picture *picture_array = (Picture *)malloc(sizeof(Picture) * num_pictures);

        // Initialize each picture in the array
        for (int i = 0; i < num_pictures; i++)
        {
            // Read id and size of the picture from stdin
            fscanf(inputFile, "%d", &picture_array[i].id);
            fscanf(inputFile, "%d", &picture_size);

            // Set size of the picture to picture_size^2
            picture_array[i].size = picture_size * picture_size;

            // Allocate memory for the picture array
            picture_array[i].picture = (int *)malloc(sizeof(int) * picture_array[i].size);

            // Initialize each pixel in the picture
            for (int j = 0; j < picture_array[i].size; j++)
            {
                fscanf(inputFile, "%d", &picture_array[i].picture[j]);
            }
        }

        // Read number of objects from stdin
        fscanf(inputFile, "%d", &num_objects);

        // Allocate memory for the object array
        Object *object_array = (Object *)malloc(sizeof(Object) * num_objects);

        // Initialize each object in the array
        for (int i = 0; i < num_objects; i++)
        {
            // Read id and size of the object from stdin
            fscanf(inputFile, "%d", &object_array[i].id);
            fscanf(inputFile, "%d", &object_size);

            // Set size of the object to picture_size^2
            object_array[i].size = object_size * object_size;

            // Compute width and height of the object

            // Allocate memory for the object array
            object_array[i].object = (int *)malloc(sizeof(int) * object_array[i].size);

            // Initialize each pixel in the object
            for (int j = 0; j < object_array[i].size; j++)
            {
                fscanf(inputFile, "%d", &object_array[i].object[j]);
            }
        }

        // Send initial data to worker threads
        int sent_pairs = 0;
        for (int worker_rank = 1; worker_rank < world_size; worker_rank++)
        {
            int picture_index = sent_pairs / num_objects;
            int object_index = sent_pairs % num_objects;

            MPI_Send(&picture_array[picture_index], 1, MPI_Picture, worker_rank, 0, MPI_COMM_WORLD);
            MPI_Send(picture_array[picture_index].picture, picture_array[picture_index].size, MPI_INT, worker_rank, 1, MPI_COMM_WORLD);
            MPI_Send(&object_array[object_index], 1, MPI_Object, worker_rank, 0, MPI_COMM_WORLD);
            MPI_Send(object_array[object_index].object, object_array[object_index].size, MPI_INT, worker_rank, 1, MPI_COMM_WORLD);

            sent_pairs++;
        }

        // Process results and send new data
        PictureResult *picture_results = NULL;
        PictureResult *pr;
        int received_results = 0;
        int worker_rank;
        MPI_Status status;

        while (received_results < num_pictures * num_objects)
        {
            MPI_Recv(&result, sizeof(Result), MPI_BYTE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            worker_rank = status.MPI_SOURCE;

            if (result.found)
            {
                pr = NULL;
                int pic_id = result.picture;

                // Find the picture in the hashmap
                HASH_FIND_INT(picture_results, &pic_id, pr);

                // If the picture is not in the hashmap, add it
                if (pr == NULL)
                {
                    pr = (PictureResult *)malloc(sizeof(PictureResult));
                    pr->picture_id = pic_id;
                    pr->count = 0;
                    pr->results = NULL;
                    HASH_ADD_INT(picture_results, picture_id, pr);
                }

                // Add the result to the picture
                pr->results = (Result *)realloc(pr->results, (pr->count + 1) * sizeof(Result));
                pr->results[pr->count] = result;
                pr->count++;
            }

            // free(results);

            // Send new data if available
            if (sent_pairs < num_pictures * num_objects)
            {
                int picture_index = sent_pairs / num_objects;
                int object_index = sent_pairs % num_objects;

                MPI_Send(&picture_array[picture_index], 1, MPI_Picture, worker_rank, 0, MPI_COMM_WORLD);
                MPI_Send(picture_array[picture_index].picture, picture_array[picture_index].size, MPI_INT, worker_rank, 1, MPI_COMM_WORLD);
                MPI_Send(&object_array[object_index], 1, MPI_Object, worker_rank, 0, MPI_COMM_WORLD);
                MPI_Send(object_array[object_index].object, object_array[object_index].size, MPI_INT, worker_rank, 1, MPI_COMM_WORLD);
                sent_pairs++;
            }
            received_results++;
            printf("Sent %d pairs, received %d results\n", sent_pairs, received_results);
        }
        printf("All results received\n");

        for (int worker_rank = 1; worker_rank < world_size; worker_rank++)
        {
            MPI_Send(NULL, 0, MPI_BYTE, worker_rank, STOP_WORKER, MPI_COMM_WORLD);
        }
        printf("Sent stop signals\n");

        // Clean up allocated memory
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
        printf("Freed memory\n");

        // Prepare to save results
        char output_strings[num_pictures][MAX_STRING_SIZE];
        memset(output_strings, 0, sizeof(output_strings));

        // Iterate through the picture_results hashmap
        PictureResult *tmp;
        pr = NULL;

        printf("Preparing output\n");
        HASH_ITER(hh, picture_results, pr, tmp)
        {
            int idx = pr->picture_id - 1;
            if (pr->count >= 3)
            {
                int offset = snprintf(output_strings[idx], MAX_STRING_SIZE, "Picture ID: %d found Objects:", pr->picture_id);
                for (int i = 0; i < pr->count && offset < MAX_STRING_SIZE; i++)
                {
                    offset += snprintf(output_strings[idx] + offset, MAX_STRING_SIZE - offset, " %d Position(%d, %d); ", pr->results[i].object, pr->results[i].i, pr->results[i].j);
                }
            }
            else
            {
                snprintf(output_strings[idx], MAX_STRING_SIZE, "Not enough objects were found for Picture ID: %d", pr->picture_id);
            }
        }

        FILE *file = fopen(OUTPUT_FILE, "w");
        if (file == NULL)
        {
            printf("Error opening output.txt for writing\n");
            return 1; // Or handle the error as appropriate
        }

        for (int i = 0; i < num_pictures; i++)
        {
            if (output_strings[i][0] == '\0')
            {
                fprintf(file, "No objects were found for Picture ID: %d\n", i + 1);
            }
            else
            {
                fprintf(file, "%s\n", output_strings[i]);
            }
        }
        printf("Saved output into %s\nIf you ran this with make run the output file will not be printed.\n\n", OUTPUT_FILE);
        fclose(file);
    }
    else
    {
        MPI_Status status;
        MPI_Bcast(&threshold, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
        while (true)
        {
            // Check if there's a message available
            MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            // If the message is a stop signal, break from the loop
            if (status.MPI_TAG == STOP_WORKER)
            {
                break;
            }

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
            printf("Worker %d received picture with id %d and object with id %d:\n", rank, picture.id, object.id);

            result = find_overlap(picture, object, threshold);

            // Send the result to the root process
            MPI_Send(&result, sizeof(Result), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
            // free(results);
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