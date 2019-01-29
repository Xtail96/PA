#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

unsigned long cur_time() {
    struct timeb t;  
    ftime(&t);
    return (unsigned long) t.time * 1000 + t.millitm;
}

void divideByRows(long m, long n, long N)
{
    int A_id = shmget( IPC_PRIVATE, sizeof(int[m][n]), 0666 | IPC_CREAT | IPC_EXCL );
    int b_id = shmget( IPC_PRIVATE, sizeof(int[n]), 0666 | IPC_CREAT | IPC_EXCL );
    int c_id = shmget( IPC_PRIVATE, sizeof(int[m]), 0666 | IPC_CREAT | IPC_EXCL );
    
    int (*A)[n] = (int(*)[n]) shmat(A_id, NULL, 0);   
    int *b = (int*) shmat(b_id, NULL, 0);        
    int *c = (int*) shmat(c_id, NULL, 0);        

    for (long i = 0; i < n; i++) {
        b[i] = i + 1;
        for (long j = 0; j < m; j++) {
            A[j][i] = j + 1;
        }
    }
    unsigned long start = cur_time();
    for (long i = 0; i < N; i++) {
        if (fork() == 0) {
            int res;
            for (int j = (i * m) / N; j < ((i + 1)*m) / N; j++) {
                res = 0;
                for (int k = 0; k < n; k++) {
                    res += A[j][k] * b[k];
                }
                c[j] = res;
            }
            return;
        }
    }
    while(wait(NULL) > 0);
    printf("elapsed time: %lu ms\n", cur_time() - start);

    /*printf("c: \n");
    for (int i = 0; i < m; i++) {
        printf("%i ", c[i]);
    }
    printf("\n");*/

    shmctl(A_id, IPC_RMID, NULL);
    shmctl(b_id, IPC_RMID, NULL);
    shmctl(c_id, IPC_RMID, NULL);
}

void semafor(int semafor_id, int n, int i) {
    struct sembuf buf;
    buf.sem_num = i;
    buf.sem_op = n;
    buf.sem_flg = 0;
    semop(semafor_id, &buf, 1);
}

void divideByColumns(long n, long m, long N)
{
    int semafor_id = semget(IPC_PRIVATE, m, 0666 | IPC_CREAT);
    for (int i = 0; i < m; i++) {
        semafor(semafor_id, 1, i);
    }

    int A_id = shmget( IPC_PRIVATE, sizeof(int[m][n]), 0666 | IPC_CREAT | IPC_EXCL );
    int b_id = shmget( IPC_PRIVATE, sizeof(int[n]), 0666 | IPC_CREAT | IPC_EXCL );
    int c_id = shmget( IPC_PRIVATE, sizeof(int[m]), 0666 | IPC_CREAT | IPC_EXCL );

    int (*A)[n] = (int(*)[n]) shmat(A_id, NULL, 0);   
    int *b = (int*) shmat(b_id, NULL, 0);        
    int *c = (int*) shmat(c_id, NULL, 0);   

     for (long i = 0; i < n; i++) {
        b[i] = i + 1;
        for (long j = 0; j < m; j++) {
            A[j][i] = j + 1;
        }
    }
   

    unsigned long start = cur_time();
    for (long i = 0; i < N; i++) {
        if (fork() == 0) {
            for (int j = 0; j < m; j++) {
                int res = 0;
                for (int k = (i * n) / N; k < ((i + 1)*n) / N; k++) {
                    res += A[j][k] * b[k];
                }
                semafor(semafor_id, -1, j);
                c[j] += res;
                semafor(semafor_id, 1, j);
            }
            return;
        }
    }
    while(wait(NULL) > 0);
    printf("elapsed time: %lu ms\n", cur_time() - start);
    /*printf("c: \n");
    for (int i = 0; i < m; i++) {
        printf("%i ", c[i]);
    }
    printf("\n");*/

    shmctl(A_id, IPC_RMID, NULL);
    shmctl(b_id, IPC_RMID, NULL);
    shmctl(c_id, IPC_RMID, NULL);
    semctl(semafor_id, 0, IPC_RMID, NULL);
}

int main(int argc, char *argv[])
{ 
    if(argc != 5) {
        printf("Illegal arguments\n");
        return 1;
    }
    long m = atoi(argv[1]);
    long n = atoi(argv[2]);
    long N = atoi(argv[3]);
    long type = atoi(argv[4]);

    if(type == 0)
    {
        divideByRows(m, n, N);
    }
    else
    {
        divideByColumns(m, n, N);
    }
    return 0;
}