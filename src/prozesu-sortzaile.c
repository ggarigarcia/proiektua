#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "scheduler.h"
#include "main.h"

extern pthread_mutex_t mutex1;
extern pthread_cond_t cond1;
extern pthread_cond_t cond2;
extern int done,kont;

pthread_mutex_t mutex_proc;
pthread_cond_t cond_proc1;
pthread_cond_t cond_proc2;

void *timer_proc(void *arg)
{
    pthread_mutex_lock(&mutex1);

    timerArg* t_arg = (timerArg*) arg;
    int maiztasuna = t_arg->maiztasuna;
    int proc_tick = 0;

    while(1)
    {
        if(kont >= 10)
        {
            pthread_mutex_unlock(&mutex1);
            return NULL;
        }
        
        done ++;

        proc_tick++;
        if(proc_tick == maiztasuna)
        {
            proc_tick = 0;

            printf("Prozesu sortzaile\n");
            
            
            
        }
        pthread_cond_signal(&cond1);
        
        pthread_cond_wait(&cond2,&mutex1);
    }
}

void *prozesu_sortzaile(void * arg)
{
    
}