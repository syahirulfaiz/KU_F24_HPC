/* File:     
 *    pth_rand.c 
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
 *    gcc -g -Wall -o pth_rand pth_rand.c -lpthread
 *
 *
 * Run:
 *    ./pth_rand <thread_count> <array_size_1> <array_size_2>
 * Notes:  
 *     1.  Storage for array1 and array2 are dynamically allocated.
 *     2.  array1 and array2 globally shared.
 *     3.  Compile with -DDEBUG for information on generated data
 *         
*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define GET_TIME(now) {				\
   struct timeval t; \
   gettimeofday(&t, NULL); \
   now = t.tv_sec + t.tv_usec/1000000.0; \
}


// Global variables
int* array1 = NULL;
int* array2 = NULL;
int size1, size2, thread_count;

// Function prototypes
void Get_args(int argc, char* argv[], int* thread_count_p, int* size1_p, int* size2_p);
void Usage(char* prog_name);
void* findCommonElements(void* rank);

int main(int argc, char* argv[]) {
    pthread_t* thread_handles;
    double start_time, end_time;

    // Validate and parse command-line arguments
    Get_args(argc, argv, &thread_count, &size1, &size2);

    // Allocate arrays dynamically based on the input sizes
    array1 = (int*)malloc(size1 * sizeof(int));
    array2 = (int*)malloc(size2 * sizeof(int));
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

    // Allocate thread handles
    thread_handles = malloc(thread_count * sizeof(pthread_t));

    // Start timing
    GET_TIME(start_time);

    // Create threads
    for (long thread = 0; thread < thread_count; thread++) {
        pthread_create(&thread_handles[thread], NULL, findCommonElements, (void*)thread);
    }

    // Join threads
    for (long thread = 0; thread < thread_count; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }

    // Stop timing
    GET_TIME(end_time);

    // Print elapsed time
    printf("RUNNING %d threads\tElapsed time = %f seconds\n", thread_count, end_time - start_time);

    // Free allocated memory
    free(array1);
    free(array2);
    free(thread_handles);

    return 0;
}

// Function to validate and parse command-line arguments
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

// Thread function to find common elements
void* findCommonElements(void* rank) {
    long my_rank = (long)rank;
    int local_start = my_rank * size1 / thread_count;
    int local_end = (my_rank + 1) * size1 / thread_count;


    for (int i = local_start; i < local_end; i++) {
        for (int j = 0; j < size2; j++) {
            if (array1[i] == array2[j]) {
#  ifdef DEBUG
                printf("Common elements: %d\n", array1[i]);
#  endif
                break;
            }
        }
    }

    return NULL;
}