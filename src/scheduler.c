#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "scheduler.h"
#include "main.h"
#include "prozesu-sortzaile.h"

extern pthread_mutex_t mutex1;
extern pthread_cond_t cond1;
extern pthread_cond_t cond2;
extern uint done,abisu;
extern machine *makina;

int politika;

/* pthread_mutex_t mutex_sched;
pthread_cond_t cond_sched;
int lana = 0;

void *scheduler(void *arg){
    //pthread bat mutexekin, aldi berean bi dei egiten badira (erloju,timer_sched) soilik bakarra hartzeko
    while(1){
        pthread_mutex_lock(&mutex_sched);
        while(lana == 0)
        {
            pthread_cond_wait(&cond_sched,&mutex_sched);
        }

    }
} */

int shortest_job_first(pcb_ilara *ilara)
{
    pcb *current = ilara->head;
    pcb *min = current;
    pcb *prev = NULL;
    pcb *prev_min = NULL;

    while(current != NULL)
    {
        if(current->info->exek_denb < min->info->exek_denb)
        {
            min = current;
            prev_min = prev;
        }
        prev = current;
        current = (pcb *) current->hurrengoa;
    }

    if(min == ilara->head)
    {
        ilara->head = (pcb *) min->hurrengoa;
    } else{
        prev_min->hurrengoa = min->hurrengoa;
    }

    if(min == ilara->tail)
    {
        ilara->tail = prev_min;
    }

    return 0;
}


//TODO 99% mutex bat jarri, ilararen atzipen esklusiboa bermatzeko
int ordenatu_ilara(pcb_ilara *ilara, int politika)
{
    switch(politika)
    {
        case FCFS:

            break;
        case SJF:
            shortest_job_first(ilara);
            break;
        default:
            return 1;
            break;
    }

    return 0;
}

void* disptacher(void *arg){
    
}

void *timer_sched(void *arg)
{
    timerArgs* t_args = (timerArgs*) arg;
    uint maiztasuna = t_args->maiztasuna;
    uint sched_tick = 0;

    pthread_mutex_lock(&mutex1);
    while(1)
    {
        if(abisu >= TTL)
        {
            pthread_mutex_unlock(&mutex1);
            return NULL;
        }

        done++;
        
        sched_tick++;
        if(sched_tick == maiztasuna)
        {
            sched_tick = 0;

            printf("(SCHED)\n");
            //scheduler mutex, cond
        }
        pthread_cond_signal(&cond1);
        
        pthread_cond_wait(&cond2,&mutex1);
    }
}