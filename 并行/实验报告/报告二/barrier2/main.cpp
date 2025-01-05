        #include<iostream>
        #include<pthread.h>
        #include <stdlib.h>
        #define num 10
        using namespace std;
        typedef struct thread_item{
            int thread_id;
        }Thread_PARAM;
        pthread_mutex_t mutexs;
        pthread_mutex_t print;
        pthread_cond_t cond;
        int counter=0;
        void * func(void * args) {
            Thread_PARAM *p = (Thread_PARAM *) args;

            pthread_mutex_lock(&print);
            fprintf(stdout, "Thread %d has entered step 1.\n", p->thread_id);
            pthread_mutex_unlock(&print);
            pthread_mutex_lock(&mutexs);
            counter++;
            pthread_mutex_unlock(&mutexs);
            while (counter < num);
            pthread_mutex_lock(&print);
            fprintf(stdout, "Thread %d has entered step 2.\n", p->thread_id);
            pthread_mutex_unlock(&print);
            pthread_exit(NULL);
        }
        int main(){
            int j=0;
            while(j<20){
                pthread_mutex_init(&mutexs, NULL);
                pthread_mutex_init(&print, NULL);
                pthread_cond_init(&cond, NULL);
                pthread_t thread[num];
                Thread_PARAM thread_param[num];
                for(int i=0;i<num;i++){
                    thread_param[i].thread_id = i;
                    pthread_create(&thread[i],NULL,func,(void*)&thread_param[i]);
                }
                for(int j=0;j<num;j++){
                    pthread_join(thread[j],NULL);
                }
                j++;
            }
            return 0;
        }
