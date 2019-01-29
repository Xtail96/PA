#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <omp.h>

unsigned long currentTime()
{
    struct timeb t;
    ftime(&t);
    return (unsigned long) t.time * 1000 + t.millitm;
}

void withReduction(int stepsCount, int threadsCount)
{
    unsigned long startTime = currentTime();
    double result = 0;
    #pragma omp parallel num_threads(threadsCount)
    {
        #pragma omp for reduction(+:result)
        for (int i = 1; i <= stepsCount; i++)
        {
            result += (double) ((pow(-1, i + 1) * 4) / (2 * i - 1));
        }
    }
    printf("elapsed time: %lu ms\n", currentTime() - startTime);
    printf("pi = %f\n", result);
}

void withoutReduction(int stepsCount, int threadsCount)
{
    unsigned long startTime = currentTime();
    double result = 0;
    #pragma omp parallel num_threads(threadsCount) 
    {
        #pragma omp for
        for (int i = 1; i <= stepsCount; i++){
            #pragma omp critical
            {
                result += (double) ((pow(-1, i + 1) * 4) / (2 * i - 1));
            }
        }
    }
    printf("elapsed time: %lu ms\n", currentTime() - startTime);
    printf("pi = %f\n", result);
}

int main(int argc, char *argv[])
{
    int threadsCount;
    int stepsCount;
    int type;
    if (argc != 4)
    {
        printf("Illegal arguments\n");
        return 1;
    }

    threadsCount = atoi(argv[1]);
    stepsCount = atoi(argv[2]);
    printf("Threads Count (P) = %i\n", threadsCount);
    printf("Steps Count (N) = %i\n", stepsCount);

    if (threadsCount < 1 && stepsCount < 1)
    {
        printf("Illegal arguments\n");
        return 1;
    }

    type = atoi(argv[3]);
    if(type == 0)
    {
        withReduction(stepsCount, threadsCount);
    }
    else
    {
        withoutReduction(stepsCount, threadsCount);
    }
    
    return 0;
}
