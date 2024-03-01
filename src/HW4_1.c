#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define X_SIZE 1000
#define Y_SIZE 1000
#define Cx 0.125
#define Cy 0.11
#define CMin 200
#define CMax 800
#define TIMESTEPS 4000
#define PRINT_STEPS 200
#define MASTER 0

void initialize_grid(float grid[X_SIZE][Y_SIZE]);
void print_cycle_temperatures(float grid[X_SIZE][Y_SIZE], int cycle);
void update_grid(float (*grid)[Y_SIZE], float (*new_grid)[Y_SIZE], int start_row, int end_row);

int main(int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    float (*grid)[Y_SIZE] = (float (*)[Y_SIZE])malloc(X_SIZE * Y_SIZE * sizeof(float));
    float (*new_grid)[Y_SIZE] = (float (*)[Y_SIZE])malloc(X_SIZE * Y_SIZE * sizeof(float));

    if (rank == MASTER) {
        initialize_grid(grid);
    }

    MPI_Bcast(grid, X_SIZE * Y_SIZE, MPI_FLOAT, MASTER, MPI_COMM_WORLD);

    int rows_per_process = X_SIZE / size;
    int start_row = rank * rows_per_process;
    int end_row = (rank == size - 1) ? X_SIZE : start_row + rows_per_process;

    double start_time = MPI_Wtime();

    for (int step = 0; step <= TIMESTEPS; ++step) {
        // Update the new_grid based on the current grid's values
        update_grid(grid, new_grid, start_row, end_row);

        // Synchronize and update the grid for the next step
        MPI_Gather(new_grid[start_row], rows_per_process * Y_SIZE, MPI_FLOAT,
                   grid, rows_per_process * Y_SIZE, MPI_FLOAT, MASTER, MPI_COMM_WORLD);

        if (rank == MASTER) {
            print_cycle_temperatures(grid, step);
        }

        // Broadcast the updated grid for the next timestep
        MPI_Bcast(grid, X_SIZE * Y_SIZE, MPI_FLOAT, MASTER, MPI_COMM_WORLD);
    }

    if (rank == MASTER) {
        double end_time = MPI_Wtime();
        printf("Execution Time: %f seconds\n", end_time - start_time);
    }
    // Free the allocated memory
    free(grid);
    free(new_grid);

    MPI_Finalize();
    return 0;
}



void initialize_grid(float grid[X_SIZE][Y_SIZE]) {
    for (int i = 0; i < X_SIZE; ++i) {
        for (int j = 0; j < Y_SIZE; ++j) {
            grid[i][j] = (i >= CMin && i <= CMax && j >= CMin && j <= CMax) ? 500.0 : 0.0;
        }
    }
}

void update_grid(float (*grid)[Y_SIZE], float (*new_grid)[Y_SIZE], int start_row, int end_row) {
    for (int i = start_row; i < end_row; ++i) {
        for (int j = 1; j < Y_SIZE - 1; ++j) {
            // Ensure we only update cells within the grid boundaries
            if (i > 0 && i < X_SIZE - 1) {
                new_grid[i][j] = grid[i][j] + 
                                 Cx * (grid[i + 1][j] + grid[i - 1][j] - 2 * grid[i][j]) +
                                 Cy * (grid[i][j + 1] + grid[i][j - 1] - 2 * grid[i][j]);
            }
        }
    }
}

void print_cycle_temperatures(float grid[X_SIZE][Y_SIZE], int cycle) {
    if (cycle % PRINT_STEPS == 0) {
        printf("Cycle: %-4d. ", cycle);
        printf("[1,1]: %f, [150,150]: %f, [400,400]: %f, [500,500]: %f, [750,750]: %f, [900,900]: %f\n",
               grid[1][1], grid[150][150], grid[400][400], grid[500][500], grid[750][750], grid[900][900]);
    }
}