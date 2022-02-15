#include <stdio.h>
#include <stdlib.h>

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
    const int WINDOW_SIZE = 3;
    const int ELEMENT_NUM = 10;

    float* nums = (float*)malloc(sizeof(float) * ELEMENT_NUM);
    for (int i = 0; i < ELEMENT_NUM; i++) {
        nums[i] = i;
    }
    // print original array
    printf("original array: ");
    for (int i = 0; i < ELEMENT_NUM; i++) {
        printf("%f,", nums[i]);
    }
    printf("\n");

    float* ma = compute_moving_avg(nums, ELEMENT_NUM, WINDOW_SIZE);
    // print moving average 
    printf("moving average: ");
    for (int i = 0; i < ELEMENT_NUM; i++) {
        printf("%f,", ma[i]);
    }
    printf("\n");
    free(ma);
    free(nums);

    return 0;
}