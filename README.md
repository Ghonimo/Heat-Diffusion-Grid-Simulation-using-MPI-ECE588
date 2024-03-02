# Heat Distribution Simulation using MPI

This repository contains a C program designed to simulate heat distribution across a 2D grid. The simulation is parallelized using the Message Passing Interface (MPI) to enhance performance and demonstrate the application of distributed computing in computational physics problems.

## Problem Description

The program models the temporal evolution of heat distribution on a 2D grid, representing a simplified scenario of heat conduction in a square plate. The simulation uses a finite difference method to approximate the heat equation, with specific initial and boundary conditions.

### Initial and Boundary Conditions

- **Grid Size**: The computational domain is a 2D grid of size 1000 x 1000 points.
- **Initial Temperature Distribution**: 
  - All points within the central region defined by coordinates (200, 200) to (800, 800) are initialized to 500 degrees.
  - The temperature of all other points is set to zero.
- **Boundary Conditions**: Points on the boundary of the grid maintain a constant temperature of zero throughout the simulation.

### Simulation Parameters

T(x,y)(t) = T(x,y)(t-1) + Cx * (T(x+1,y)(t-1) + T(x-1,y)(t-1) – 2 * T(x,y)(t-1)) 
                      + Cy * (T(x,y+1)(t-1) + T(x,y-1)(t-1) – 2 * T(x,y)(t-1))



Where `Cx` and `Cy` are constants that govern the rate of heat transfer along the x and y axes, respectively.

- **Time Steps**: The simulation progresses through 4000 time steps.
- **Reporting**: After every 200 time steps, the program prints the temperatures at specific grid points: (1, 1), (150,150), (400, 400), (500, 500), (750, 750), and (900,900).

## Implementation Details

The program is implemented in C and utilizes MPI to distribute the computation across multiple processors within a distributed memory system, aiming for a high degree of parallel speedup. The domain is divided among the MPI processes, each responsible for a portion of the grid. MPI collectives and point-to-point communication are used to synchronize boundary conditions and gather results.

## Building and Running

To compile the program with MPI, use:

```bash
mpicc -o HW4_1 HW4_1.c
```

To run the compiled binary across a specific number of processes:

```bash
mpirun -np <num_processes> ./HW4_1
```
To run in batch more, comparing the performance of with different cores, you can execute the perl script below:

```bash
./automate/batch_run.pl
```

## Output
The program outputs the temperatures of specified grid points after every 200 timesteps and displays the total runtime in nanoseconds and seconds.

## License
This project is open-sourced under the MIT License.
