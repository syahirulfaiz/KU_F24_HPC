/* File:     
 *    mpi_omp_rand.c 
 *
 * Purpose:  
 *    Find all the common elements of two very large arrays of integers 
 *    that are not necessarily of the same size. 
 *    We use rand() with seed srand(100065736) <-- our student ID, to generate array1 and array2.
 *
 * Input:
 *     None
 *
 * Output:
 *     Compile with -DDEBUG for information on generated data: (1) array1 elements, (2) array2 elements, and (3) common elements.
 *     Elapsed time for the computation.
 *
 * Compile:  
 *     mpicc -fopenmp -g -Wall -o mpi_omp_rand mpi_omp_rand.c 
 *
 * Run:
 *     export OMP_NUM_THREADS=$thread; mpiexec -n <num_of_processes> mpi_omp_rand <array_size_1> <array_size_2>
 * Notes:  
 *     1.  array1 and array2 must be divisible by comm_size (num_procs).
 *     2.  Compile with -DDEBUG for information on generated data.
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

// Function prototypes
void Get_args(int argc, char* argv[], int* size1_p, int* size2_p, int rank, int num_procs); 
void Usage(char* prog_name);
void findCommonElementsMPI(int* array1, int size1, int* array2, int size2, int rank, int num_procs);

int main(int argc, char* argv[]) {
    int size1, size2;
    int rank, num_procs;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // Validate and parse command-line arguments
    Get_args(argc, argv, &size1, &size2, rank, num_procs);

    // Allocate memory for arrays
    int* array1 = NULL;
    int* array2 = (int*)malloc(size2 * sizeof(int));

    if (rank == 0) {
        array1 = (int*)malloc(size1 * sizeof(int));
        if (array1 == NULL || array2 == NULL) {
            fprintf(stderr, "Memory allocation failed.\n");
            MPI_Finalize();
            exit(-1);
        }

        // Initialize the randomizer with a fixed value
        srand(100065736);


        // Generate random elements for the first array
#  ifdef DEBUG
        printf("Array 1: ");
#  endif
        for (int i = 0; i < size1; i++) {
            array1[i] = rand() % RAND_MAX + 1;
#  ifdef DEBUG
            printf("%d ", array1[i]);
#  endif
        }
#  ifdef DEBUG
        printf("\n");
#  endif

        // Generate random elements for the second array
#  ifdef DEBUG
        printf("Array 2: ");
#  endif
        for (int i = 0; i < size2; i++) {
            array2[i] = rand() % RAND_MAX + 1;
#  ifdef DEBUG
            printf("%d ", array2[i]);
#  endif
        }
#  ifdef DEBUG
        printf("\n");
#  endif
    }

    // Broadcast array2 to all processes
    MPI_Bcast(array2, size2, MPI_INT, 0, MPI_COMM_WORLD);

    // Scatter array1 to all processes
    int local_size = size1 / num_procs;
    int* local_array1 = (int*)malloc(local_size * sizeof(int));
    MPI_Scatter(array1, local_size, MPI_INT, local_array1, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD); 
    
    // Start timing
    double start_time = MPI_Wtime();

    // Find and print common elements
    findCommonElementsMPI(local_array1, local_size, array2, size2, rank, num_procs);

    // Stop timing
    double end_time = MPI_Wtime();
    
    double loc_elapsed, elapsed;
    loc_elapsed = end_time-start_time;
    MPI_Reduce(&loc_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        printf("RUNNING: %d PROCESSES\tEACH %d THREADS\tElapsed time = %f seconds\n", num_procs, omp_get_max_threads(), elapsed);
    }

    // Free allocated memory
    if (rank == 0) free(array1);
    free(array2);
    free(local_array1);

    // Finalize MPI
    MPI_Finalize();
    return 0;
}

// Function to validate and parse command-line arguments
void Get_args(int argc, char* argv[], int* size1_p, int* size2_p, int rank, int num_procs) {
    if (argc != 3) {
        if (rank == 0) Usage(argv[0]);
        MPI_Finalize();
        exit(-1);
    }

    *size1_p = strtol(argv[1], NULL, 10);
    *size2_p = strtol(argv[2], NULL, 10);

    if (*size1_p <= 0 || *size2_p <= 0 || *size1_p % num_procs != 0 || *size2_p % num_procs != 0) {
        if (rank == 0) {
            fprintf(stderr, "Error: Array sizes must be positive and divisible by the number of processes (%d).\n", num_procs);
            Usage(argv[0]);
        }
        MPI_Finalize();
        exit(-1);
    }
}

// Function to display usage and terminate the program
void Usage(char* prog_name) {
    fprintf(stderr, "Usage: %s <array_size_1> <array_size_2>\n", prog_name);
    fprintf(stderr, "Note: Both array sizes must be divisible by the number of processes.\n");
    exit(0);
}

// MPI function to find and print common elements
void findCommonElementsMPI(int* array1, int size1, int* array2, int size2, int rank, int num_procs) {
    int i;
    int j;
    
    // Parallel region
    #pragma omp parallel default(none) shared(array1, array2, size1, size2) private(i, j) 
    // Outer loop parallelized
    #pragma omp for schedule(static)
		
    for (i = 0; i < size1; i++) {
        for (j = 0; j < size2; j++) {
            if (array1[i] == array2[j]) {
                // Print common elements
#  ifdef DEBUG  
                printf("Common elements: %d\n", array1[i]);
#  endif 
                break;
            }
        }
    }
}