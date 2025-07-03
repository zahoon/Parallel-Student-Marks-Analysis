// Group Project 11 : Parallel Implementation in Student Marks Analysis Between 3 Subject Score using MPI and OpenMP
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <omp.h>

#define N 30

// Dataset
// English = {87, 56, 92, 60, 78, 67, 50 ,89, 75, 97}
// Math    = {67, 79, 88, 56, 98, 76, 55 ,82, 73, 80}
// Science = {88, 72, 89, 67, 52, 92, 83 ,79, 91, 77}

// main function to find global min/max using OpenMP
int main() {
    int data[N] = {
        87,56,92,60,78,67,50,89,75,97,   // English
        67,79,88,56,98,76,55,82,73,80,   // Math
        88,72,89,67,52,92,83,79,91,77    // Science
    };
    int global_min = INT_MAX, global_max = INT_MIN;
    double t0, t1;
    char subj_min[16] = "", subj_max[16] = "";

    // main OpenMP implementation
    t0 = omp_get_wtime();
    #pragma omp parallel for reduction(min: global_min) reduction(max: global_max)
    for (int i = 0; i < N; i++) {
        if (data[i] < global_min) global_min = data[i];
        if (data[i] > global_max) global_max = data[i];
    }
    t1 = omp_get_wtime();

    // Determine subject for min
    for (int i = 0; i < 10; i++)
        if (data[i] == global_min) strcpy(subj_min, "English");
    for (int i = 10; i < 20; i++)
        if (data[i] == global_min) strcpy(subj_min, "Math");
    for (int i = 20; i < 30; i++)
        if (data[i] == global_min) strcpy(subj_min, "Science");
    // Determine subject for max
    for (int i = 0; i < 10; i++)
        if (data[i] == global_max) strcpy(subj_max, "English");
    for (int i = 10; i < 20; i++)
        if (data[i] == global_max) strcpy(subj_max, "Math");
    for (int i = 20; i < 30; i++)
        if (data[i] == global_max) strcpy(subj_max, "Science");

    printf("Processor used : %d\n", omp_get_max_threads());
    printf("OpenMP: Minimum Marks = %d (%s)\n", global_min, subj_min);   // print global min w subject
    printf("OpenMP: Maximum Marks = %d (%s)\n", global_max, subj_max);   // print global max w subject
    printf("OpenMP Elapsed Time: %f s\n", t1 - t0);
    printf("\n----------------------\n");

    return 0;
}
