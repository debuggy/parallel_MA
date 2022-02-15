#include <assert.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Creates an array of random numbers. Each number has a value from 0 - 1
float* create_rand_nums(float num_elements) {
    float* rand_nums = (float*)malloc(sizeof(float) * num_elements);
    for (int i = 0; i < num_elements; i++) {
        rand_nums[i] = rand() % 100;
    }
    return rand_nums;
}

// Computes moving average with window size len
float moving_avg(float* ptrArrNumbers, float* ptrSum, int pos, int len, float nextNum) {
    // Subtract the oldest number from the prev sum, add the new number
    *ptrSum = *ptrSum - ptrArrNumbers[pos] + nextNum;
    // Assign the nextNum to the position in the array
    ptrArrNumbers[pos] = nextNum;
    // return the average
    return *ptrSum / len;
}

// compute moving average given an input array
float* compute_moving_avg(float* origin_array, int array_length, int window_size) {
    float* window_nums = (float*)malloc(sizeof(float) * window_size);
    for (int i = 0; i < window_size; i++) {
        window_nums[i] = 0;
    }
    float* moving_averages = (float*)malloc(sizeof(float) * array_length);
    int pos = 0;
    float newAvg = 0;
    float sum = 0;

    for (int i = 0; i < array_length; i++) {
        newAvg = moving_avg(window_nums, &sum, pos, window_size, origin_array[i]);
        moving_averages[i] = newAvg;
        pos++;
        if (pos >= window_size) {
            pos = 0;
        }
    }
    free(window_nums);
    return moving_averages;
}

int main(int argc, char** argv) {
    const int WINDOW_SIZE = 3;
    const int ELEMENT_NUM = 10;

    MPI_Init(NULL, NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Create a random array of elements on the root process. Its total
    // size will be ELEMENT_NUM
    float* rand_nums = NULL;
    if (world_rank == 0) {
        // rand_nums = create_rand_nums(ELEMENT_NUM);
        rand_nums = (float*)malloc(sizeof(float) * ELEMENT_NUM);
        for (int i = 0; i < ELEMENT_NUM; i++) {
            rand_nums[i] = i;
        }
        // print original array
        printf("original array: ");
        for (int i = 0; i < ELEMENT_NUM; i++) {
            printf("%f,", rand_nums[i]);
        }
        printf("\n");
    }

    // For each process, create a buffer that will hold a subset of the entire
    // array
    int num_elements_per_proc = ELEMENT_NUM / world_size;
    float* sub_rand_nums = (float*)malloc(sizeof(float) * num_elements_per_proc);
    assert(sub_rand_nums != NULL);

    // Scatter the random numbers from the root process to all processes in
    // the MPI world
    MPI_Scatter(rand_nums, num_elements_per_proc, MPI_FLOAT, sub_rand_nums, num_elements_per_proc, MPI_FLOAT, 0,
                MPI_COMM_WORLD);

    // compute moving average in each process
    float* sub_moving_averages = compute_moving_avg(sub_rand_nums, num_elements_per_proc, WINDOW_SIZE);

    // Gather all partial averages down to the root process
    float* moving_averages = NULL;
    if (world_rank == 0) {
        moving_averages = (float*)malloc(sizeof(float) * ELEMENT_NUM);
    }
    MPI_Gather(sub_moving_averages, num_elements_per_proc, MPI_FLOAT, moving_averages, num_elements_per_proc, MPI_FLOAT,
               0, MPI_COMM_WORLD);

    // post process the boundary condition
    if (world_rank == 0) {
        for (int i = 0; i < world_size - 1; i++) {
            float* sub_array = (float*)malloc(sizeof(float) * 2 * (WINDOW_SIZE - 1));
            for (int j = 0; j < 2 * (WINDOW_SIZE - 1); j++) {
                sub_array[j] = rand_nums[(i + 1) * num_elements_per_proc - WINDOW_SIZE + 1 + j];
            }
            float* bound_moving_averages = compute_moving_avg(sub_array, 2 * (WINDOW_SIZE - 1), WINDOW_SIZE);
            for (int j = 0; j < (WINDOW_SIZE - 1); j++) {
                moving_averages[(i + 1) * num_elements_per_proc + j] = bound_moving_averages[WINDOW_SIZE - 1 + j];
            }
            free(bound_moving_averages);
        }
        printf("final result: ");
        for (int i = 0; i < ELEMENT_NUM; i++) {
            printf("%f,", moving_averages[i]);
        }
        printf("\n");
    }

    // Clean up
    if (world_rank == 0) {
        free(rand_nums);
    }
    free(sub_rand_nums);
    free(sub_moving_averages);
    free(moving_averages);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
}