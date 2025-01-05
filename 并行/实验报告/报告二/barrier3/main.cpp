#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREADS 10
typedef struct{
int threadId;
}threadParm_t;
pthread_barrier_t barrier;
void *threadFunc(void *parm)
{
threadParm_t *p = (threadParm_t *) parm;
fprintf(stdout, "Thread %d has entered step 1.\n", p->threadId);
pthread_barrier_wait(&barrier);
fprintf(stdout, "Thread %d has entered step 2.\n", p->threadId);
pthread_exit(NULL);
}
int main(int argc, char *argv[])
{
    int j=0;
    while(j<20){
        pthread_barrier_init(&barrier, NULL, NUM_THREADS);
        pthread_t thread[NUM_THREADS];
        threadParm_t threadParm[NUM_THREADS];
        int i;
        for (i=0; i<NUM_THREADS; i++)
        {
        threadParm[i].threadId = i;
        pthread_create(&thread[i], NULL, threadFunc, (void
        *)&threadParm[i]);
        }
        for (i=0; i<NUM_THREADS; i++)
        {
        pthread_join(thread[i], NULL);
        }
        pthread_barrier_destroy(&barrier);
        j++;
    }
return 0;
}
