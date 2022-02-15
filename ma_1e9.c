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

int main(int argc, char* argv[]) {
    const int WINDOW_SIZE = 120;
    const int ELEMENT_NUM = 1e9;
    start = time(NULL);

    // Seed the random number generator to get different results each time
    srand(time(NULL));

    // Create a random array of elements on the root process. Its total
    // size will be ELEMENT_NUM
    float* rand_nums = create_rand_nums(ELEMENT_NUM);

    float* ma = compute_moving_avg(rand_nums, ELEMENT_NUM, WINDOW_SIZE);

    printf("moving average result:");
    for (int i = 0; i < ELEMENT_NUM; i++) {
        if (i % (WINDOW_SIZE * 10000) == 0) {
            printf("%f,", ma[i]);
        }
    }
    printf("\n");

    end = time(NULL);
    printf("runtime = %ds\n", difftime(end, start));

    return 0;
}