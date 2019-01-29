#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <pthread.h>

int *array;

void quickSort(int array[], int first, int last) {
    if (first < last)
    {
        int left = first, right = last, middle = array[(left + right) / 2];
        do
        {
            while (array[left] < middle) left++;
            while (array[right] > middle) right--;
            if (left <= right)
            {
                int tmp = array[left];
                array[left] = array[right];
                array[right] = tmp;
                left++;
                right--;
            }
        } while (left <= right);
        quickSort(array, first, right);
        quickSort(array, left, last);
    }
}

void printArray(int* array, int size) {
    for (int i=0; i < size; i++)
        printf("%i ", array[i]);
    printf("\n");
}

void* merge(void *arg) {
    int start = ((int *) arg)[0];
    int end = ((int *) arg)[1];
    int mid = (start + end) / 2 + 1;
    int buf_size = end - start + 1;
    int buf[buf_size];
    for(int i = 0; i < buf_size; i++) {
        buf[i] = 0;
    }
    int i1 = start;
    int i2 = mid;
    for(int i = 0; i < buf_size; i++) {
        if(i1 == mid) {
            buf[i] = array[i2++];
            continue;
        }
        if(i2 == end + 1) {
            buf[i] = array[i1++];
            continue;
        }
        if(array[i1] < array[i2]) {
            buf[i] = array[i1++];
        } else {
            buf[i] = array[i2++];
        }
    }
    for(int i = start, j = 0; i <= end; i++, j++) {
        array[i] = buf[j];
    }
}

unsigned long currentTime() {
    struct timeb t;
    ftime(&t);
    return (unsigned long) t.time * 1000 + t.millitm;
}

int main(int argc, char *argv[]) {
    if(argc != 3){
        printf("Illegal arguments!\n");
        return 1;
    }

    int arraySize = atoi(argv[1]);
    int threadsCount = atoi(argv[2]);

    if(arraySize < 1 || threadsCount < 4){
        printf("Illegal arguments!\n");
        return 1;
    }

    array = (int*) malloc(arraySize * sizeof(int));

    for(int i = 0; i < arraySize; i++) {
        array[i] = rand() % 100;
    }

    int numberOfGroups = threadsCount * 2;
    int groupSize = arraySize / numberOfGroups;
    int j;
    int i = 0;
    for(int k = 0; k < numberOfGroups; k++, i = j + 1) {
        j = i + groupSize - (k > arraySize % numberOfGroups - 1);
        quickSort(array, i, j);
    }
    unsigned long start = currentTime();

    for(int i = 0; i < arraySize; i++) {

        int se[2*threadsCount];
        pthread_t threads[threadsCount];
        if(i % 2 == 0) {
            int l;
            int f = 0;
            for(int j = 0; j < threadsCount; j++, f = l + 1) {
                se[j*2] = f;
                int next_f = f + groupSize - (j * 2 > arraySize % numberOfGroups - 1) + 1;
                l = next_f + groupSize - (j * 2 + 1 > arraySize % numberOfGroups - 1);
                se[j*2 + 1] = l;
                pthread_create(&threads[j], NULL, merge, &se[j*2]);
            }
            for(int j = 0; j < threadsCount; j++) {
                pthread_join(threads[j], NULL);
            }
        } else {
            int l;
            int f = groupSize + 1;
            for(int j = 0; j < threadsCount - 1; j++, f = l + 1) {
                se[j*2] = f;
                int next_f = f + groupSize - (j * 2 + 1 > arraySize%numberOfGroups - 1) + 1;
                l = next_f + groupSize - (j * 2 + 2 > arraySize%numberOfGroups - 1);
                se[j*2+1] = l;
                pthread_create(&threads[j], NULL, merge, &se[j*2]);
            }
            for(int j = 0; j < threadsCount-1; j++) {
                pthread_join(threads[j], NULL);
            }
        }
    }
    printArray(array, arraySize);
    printf("elapsed time: %lu ms\n", currentTime() - start);
    return 0;
}
