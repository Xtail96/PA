#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <pthread.h>
#include <algorithm>

int *array;

void printArray(int* array, int size) {
    for (int i=0; i < size; i++)
        printf("%i ", array[i]);
    printf("\n");
}

unsigned long currentTime() {
    struct timeb t;
    ftime(&t);
    return (unsigned long) t.time * 1000 + t.millitm;
}

int main(int argc, char *argv[]) {
    if(argc != 2){
        printf("Illegal arguments!\n");
        return 1;
    }

    int arraySize = atoi(argv[1]);

    if(arraySize < 1){
        printf("Illegal arguments!\n");
        return 1;
    }

    array = (int*) malloc(arraySize * sizeof(int));

    for(int i = 0; i < arraySize; i++) {
        array[i] = rand() % 100;
    }

    unsigned long startTime = currentTime();
    std::sort(array, array + arraySize);

    printf("elapsed time: %lu ms\n", currentTime() - startTime);
    return 0;
}
