
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
int counter = 0;
#define NUM_THREADS 10
typedef struct {
    int threadId;
} threadParm_t;
sem_t sem_count;
sem_t sem_barrier;

void* threadFunc(void* parm)
{
    threadParm_t* p = (threadParm_t*)parm;
    fprintf(stdout, "Thread %d has entered step 1.\n", p->threadId);
    sem_wait(&sem_count);
    if (counter == NUM_THREADS - 1)
    {
        counter = 0;
        sem_post(&sem_count);
        for (int i = 0; i < NUM_THREADS - 1; i++) sem_post(&sem_barrier);
    }
    else
    {
        counter++;
        sem_post(&sem_count);
        sem_wait(&sem_barrier);
    }

    fprintf(stdout, "Thread %d has entered step 2.\n", p->threadId);
    pthread_exit(NULL);
}
int main(int argc, char* argv[])
{
    int j=0;
    while(j<20){
        sem_init(&sem_count, 0, 1);
        sem_init(&sem_barrier, 0, 0);
        pthread_t thread[NUM_THREADS];
        threadParm_t threadParm[NUM_THREADS];
        int i;
        for (i = 0; i < NUM_THREADS; i++)
        {
            threadParm[i].threadId = i;
            pthread_create(&thread[i], NULL, threadFunc, (void *)&threadParm[i]);
        }
        for (i = 0; i < NUM_THREADS; i++)
        {
            pthread_join(thread[i], NULL);
        }
        sem_destroy(&sem_count);
        sem_destroy(&sem_barrier);
        j++;
    }
    return 0;
}
