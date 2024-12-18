#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "scheduler.h"
#include "main.h"

extern pthread_mutex_t mutex1;
extern pthread_cond_t cond1;
extern pthread_cond_t cond2;
extern uint done,kont;
extern machine *makina;

void *timer_sched(void *arg)
{
    pthread_mutex_lock(&mutex1);

    timerArgs* t_args = (timerArgs*) arg;
    uint maiztasuna = t_args->maiztasuna;
    uint sched_tick = 0;

    while(1)
    {
        if(kont >= TTL)
        {
            pthread_mutex_unlock(&mutex1);
            return NULL;
        }

        done++;
        
        sched_tick++;
        if(sched_tick == maiztasuna)
        {
            sched_tick = 0;

            printf("Scheduler\n");
            //scheduler mutex, cond
        }
        pthread_cond_signal(&cond1);
        
        pthread_cond_wait(&cond2,&mutex1);
    }
}

void *scheduler(void *arg){
    //pthread bat mutexekin, aldi berean bi dei egiten badira (erloju,timer_sched) soilik bakarra hartzeko
}

void* disptacher(void *arg){
    
}