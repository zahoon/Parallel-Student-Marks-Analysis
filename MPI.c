#include <mpi.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

#define TOTAL 30

int main(int argc, char *argv[]) {
    int rank, size, i;
    int data[TOTAL], *local;
    int *sendcounts = NULL, *displs = NULL;
    int local_n;
    int local_min = INT_MAX, local_max = INT_MIN;
    int global_min, global_max;
    double t0, t1;
    char subj_min[16] = "", subj_max[16] = "";

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int chunk = TOTAL / size;
    int rem = TOTAL % size;

    if (rank == 0) {
        int eng[10] = {87,56,92,60,78,67,50,89,75,97};
        int mth[10] = {67,79,88,56,98,76,55,82,73,80};
        int sci[10] = {88,72,89,67,52,92,83,79,91,77};
        for (i = 0; i < 10; i++) data[i] = eng[i];
        for (i = 0; i < 10; i++) data[10+i] = mth[i];
        for (i = 0; i < 10; i++) data[20+i] = sci[i];

        sendcounts = malloc(size * sizeof(int));
        displs = malloc(size * sizeof(int));
        int offset = 0;
        for (i = 0; i < size; i++) {
            sendcounts[i] = chunk + (i < rem ? 1 : 0);
            displs[i] = offset;
            offset += sendcounts[i];
        }
    }

    local_n = chunk + (rank < rem ? 1 : 0);
    local = malloc(local_n * sizeof(int));

    MPI_Scatterv(data, sendcounts, displs, MPI_INT, local, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // find min and max
    for (i = 0; i < local_n; i++) {
        if (local[i] < local_min) local_min = local[i];
        if (local[i] > local_max) local_max = local[i];
    }

    t0 = MPI_Wtime();
    MPI_Reduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
    t1 = MPI_Wtime();

    // Determine the min max subject names
    if (rank == 0) {
        for (i = 0; i < 10; i++) if (data[i] == global_min) strcpy(subj_min, "English");
        for (i = 10; i < 20; i++) if (data[i] == global_min) strcpy(subj_min, "Math");
        for (i = 20; i < 30; i++) if (data[i] == global_min) strcpy(subj_min, "Science");

        for (i = 0; i < 10; i++) if (data[i] == global_max) strcpy(subj_max, "English");
        for (i = 10; i < 20; i++) if (data[i] == global_max) strcpy(subj_max, "Math");
        for (i = 20; i < 30; i++) if (data[i] == global_max) strcpy(subj_max, "Science");

        printf("\nProcessor used : ( %d )\n", size);
        printf("Minimum Marks = %d (%s)\n", global_min, subj_min);
        printf("Maximum Marks = %d (%s)\n", global_max, subj_max);
        printf("Elapsed Time: %f s\n", t1 - t0);
        printf("--------------------------\n");
    }

    free(local);
    if (rank == 0) {
        free(sendcounts);
        free(displs);
    }

    MPI_Finalize();
    return 0;
}
