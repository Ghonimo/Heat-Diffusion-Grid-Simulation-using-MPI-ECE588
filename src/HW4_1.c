#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define X_SIZE 1000
#define Y_SIZE 1000
#define Cx 0.125
#define Cy 0.11
#define CMin 200
#define CMax 800
#define M_NPROCS 16
#define TIMESTEPS 4000
#define PRINT_STEPS 200

float grid[X_SIZE][Y_SIZE];
float new_grid[X_SIZE][Y_SIZE];

void initialize_grid() {
    for (int i = 0; i < X_SIZE; ++i) {
        for (int j = 0; j < Y_SIZE; ++j) {
            if (i >= CMin && i <= CMax && j >= CMin && j <= CMax) {
                grid[i][j] = 500.0;
            } else {
                grid[i][j] = 0.0;
            }
        }
    }
}

void update_temperature(int start_row, int end_row) {
    for (int i = start_row; i < end_row; ++i) {
        for (int j = 1; j < Y_SIZE - 1; ++j) {
            if (i > 0 && i < X_SIZE - 1) {
                new_grid[i][j] = grid[i][j] + Cx * (grid[i + 1][j] + grid[i - 1][j] - 2 * grid[i][j]) + Cy * (grid[i][j + 1] + grid[i][j - 1] - 2 * grid[i][j]);
            }
        }
    }
}

int main(int argc, char** argv) {
    int rank, size;
    double start_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rows_per_process = X_SIZE / size;
    int extra_rows = X_SIZE % size;
    int my_start_row = rank * rows_per_process + (rank < extra_rows ? rank : extra_rows);
    int my_end_row = my_start_row + rows_per_process + (rank < extra_rows);

    if (rank == 0) {
        initialize_grid();
        start_time = MPI_Wtime();
    }

    // Broadcast the initial grid to all processes
    MPI_Bcast(grid, X_SIZE*Y_SIZE, MPI_FLOAT, 0, MPI_COMM_WORLD);

    for (int step = 0; step < TIMESTEPS; ++step) {
        update_temperature(my_start_row, my_end_row);

        // Gather the updated temperatures back to the master process
        MPI_Gather(new_grid[my_start_row], (my_end_row - my_start_row) * Y_SIZE, MPI_FLOAT, grid, (my_end_row - my_start_row) * Y_SIZE, MPI_FLOAT, 0, MPI_COMM_WORLD);

        if (rank == 0 && (step + 1) % PRINT_STEPS == 0) {
            printf("Temperature at [1,1]: %f\n", grid[1][1]);
            printf("Temperature at [150,150]: %f\n", grid[150][150]);
            printf("Temperature at [400,400]: %f\n", grid[400][400]);
            printf("Temperature at [500,500]: %f\n", grid[500][500]);
            printf("Temperature at [750,750]: %f\n", grid[750][750]);
            printf("Temperature at [900,900]: %f\n", grid[900][900]);            // Add more print statements as needed
        }

        // Ensure all processes have finished updating before proceeding to the next step
        MPI_Barrier(MPI_COMM_WORLD);
    }

    if (rank == 0) {
        end_time = MPI_Wtime();
        printf("Execution Time: %f seconds\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}
