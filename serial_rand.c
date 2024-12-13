#include <stdio.h>
#include <stdlib.h>

// define functions 
void Get_args(int argc, char* argv[], int* size1_p, int* size2_p);
void Usage(char* prog_name);
void findCommonElementsArrays(int arr1[], int size1, int arr2[], int size2);

int main(int argc, char* argv[]) {
    int size1, size2;

    // check and parse command-line arguments
    Get_args(argc, argv, &size1, &size2);

    // Allocate arrays dynamically based on the input sizes
    int* array1 = (int*)malloc(size1 * sizeof(int));
    int* array2 = (int*)malloc(size2 * sizeof(int));

    if (array1 == NULL || array2 == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    // Initialize the randomizer with a fixed value
    //srand(100065736);  //we use srand=100065736 for all JOB SUBMISSION in our report (Serial, OMP, Pthread, MPI, Hybrid) 
    srand(10000);        //we use srand=10000 for all test screenshot in our report (Serial, OMP, Pthread, MPI, Hybrid)  
    
    // Generate random elements for the first array
    printf("Array 1: ");
    for (int i = 0; i < size1; i++) {
        array1[i] = rand() % 100 + 1;
        printf("%d ", array1[i]);
    }
    printf("\n");

    // Generate random elements for the second array
    printf("Array 2: ");
    for (int i = 0; i < size2; i++) {
        array2[i] = rand() % 100 + 1;
        printf("%d ", array2[i]);
    }
    printf("\n");

    // Find and print common elements
    findCommonElementsArrays(array1, size1, array2, size2);

    // Free allocated memory
    free(array1);
    free(array2);

    return 0;
}

// Function to check and parse command-line arguments
void Get_args(int argc, char* argv[], int* size1_p, int* size2_p) {
    if (argc != 3) Usage(argv[0]);
    *size1_p = strtol(argv[1], NULL, 10);
    *size2_p = strtol(argv[2], NULL, 10);
    if (*size1_p <= 0 || *size2_p <= 0) Usage(argv[0]);
}

// Function to display usage and terminate the program
void Usage(char* prog_name) {
    fprintf(stderr, "Usage: %s <size1> <size2>\n", prog_name);
    exit(0);
}

// Function to find and print common elements
void findCommonElementsArrays(int* arr1, int size1, int* arr2, int size2) {
    for (int i = 0; i < size1; i++) {
        for (int j = 0; j < size2; j++) {
            if (arr1[i] == arr2[j]) {
                printf("Common elements: %d\n", arr1[i]);
                break; // Move to the next element in arr1 after finding a common element
            }
        }
    }
    printf("\n");
}