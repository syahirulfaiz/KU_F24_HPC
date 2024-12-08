/* File:     
 *    omp_rand.c 
 *
 * Purpose:  
 *    Find all the common elements of two very large arrays of integers 
 *    that are not necessarily of the same size. 
 *    We use rand() with seed srand(100065736) <-- our student ID, to generate array1 and array2
 *
 * Input:
 *     None
 *
 * Output:
 *     Compile with -DDEBUG for information on generated data: (1) array1 elements, (2) array2 elements, and (3) common elements
 *     Elapsed time for the computation
 *
 * Compile:  
 *    gcc -g -Wall -fopenmp -o omp_rand  omp_rand.c 
 *
 * Run:
 *    ./omp_rand <thread_count> <array_size_1> <array_size_2>
 * Notes:  
 *     1.  Storage for array1 and array2 are dynamically allocated.
 *     2.  array1 and array2 globally shared.
 *     3.  Compile with -DDEBUG for information on generated data
 *         
*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Function prototypes
void Get_args(int argc, char* argv[], int* thread_count_p, int* size1_p, int* size2_p); 
void Usage(char* prog_name);
void findCommonElementsArraysOMP(int* arr1, int size1, int* arr2, int size2);

int main(int argc, char* argv[]) {
    int thread_count;
    int size1, size2;

    // check and parse command-line arguments
    Get_args(argc, argv, &thread_count, &size1, &size2);

    // Allocate arrays dynamically based on the input sizes
    int* array1 = (int*)malloc(size1 * sizeof(int));
    int* array2 = (int*)malloc(size2 * sizeof(int));

    if (array1 == NULL || array2 == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
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

    // Start timing the OpenMP execution
    double start_time = omp_get_wtime();

    // Find and print common elements using OpenMP
    findCommonElementsArraysOMP(array1, size1, array2, size2);

    // Stop timing
    double end_time = omp_get_wtime();
    printf("RUNNING %d threads\tElapsed time = %f seconds\n", omp_get_max_threads(),end_time - start_time);

    // Free allocated memory
    free(array1);
    free(array2);

    return 0;
}

// Function to check and parse command-line arguments
void Get_args(int argc, char* argv[], int* thread_count_p, int* size1_p, int* size2_p) {
    if (argc != 4) Usage(argv[0]);
    *thread_count_p = strtol(argv[1], NULL, 10);
    *size1_p = strtol(argv[2], NULL, 10);
    *size2_p = strtol(argv[3], NULL, 10);
    if (*thread_count_p <= 0 || *size1_p <= 0 || *size2_p <= 0) Usage(argv[0]);
}

// Function to display usage and terminate the program
void Usage(char* prog_name) {
    fprintf(stderr, "Usage: %s <thread_count> <size1> <size2>\n", prog_name);
    exit(0);
}

// OpenMP function to find and print common elements
void findCommonElementsArraysOMP(int* arr1, int size1, int* arr2, int size2) {
    int i;
    int j;
    // Parallel region
    #pragma omp parallel default(none) shared(arr1, arr2, size1, size2) private(i, j) 

        // Outer loop parallelized
        #pragma omp for schedule(static)
        for ( i = 0; i < size1; i++) {
            for ( j = 0; j < size2; j++) {
                if (arr1[i] == arr2[j]) {
                    // printing of the common element
#  ifdef DEBUG  
                        printf("Common elements: %d\n", arr1[i]);
#  endif 
                    break; // Move to the next element in arr1 after finding a match
                }
            }
        }
#  ifdef DEBUG
    printf("\n");
#  endif
}  