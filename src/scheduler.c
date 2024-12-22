#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "scheduler.h"
#include "main.h"
#include "prozesu-sortzaile.h"

extern pthread_mutex_t mutex1;
extern pthread_cond_t cond1,cond2;

extern uint done,abisu;
extern machine *makina;
extern pcb_ilara *pcb_ilara_nagusia;

int politika; //FCFS, SJF, RR

int shortest_job_first(pcb_ilara *ilara)
{
    if (ilara == NULL || ilara->head == NULL) {
        return 1; // Error: ilara is NULL or empty
    }

    int swapped;
    pcb *ptr1;
    pcb *lptr = NULL;

    // Bubble sort
    do {
        swapped = 0;
        ptr1 = ilara->head;

        while (ptr1->hurrengoa != lptr) {
            pcb *next = (pcb *)ptr1->hurrengoa;
            if (ptr1->info->exek_denb > next->info->exek_denb) {
                // Swap the info pointers
                pcb_info *temp = ptr1->info;
                ptr1->info = next->info;
                next->info = temp;
                swapped = 1;
            }
            ptr1 = next;
        }
        lptr = ptr1;
    } while (swapped);

    return 0; // Success
}


//TODO mutex bat jarri, ilararen atzipen esklusiboa bermatzeko
int ilara_ordenatu(pcb_ilara *ilara)
{
    switch(politika)
    {
        case FCFS:

            break;
        case SJF:
            shortest_job_first(ilara);
            break;
        default:
            printf("(WARNING) Politika okerra\n");
            return 1;
            break;
    }

    return 0;
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
            ilara_ordenatu(pcb_ilara_nagusia);
        }
        pthread_cond_signal(&cond1);
        
        pthread_cond_wait(&cond2,&mutex1);
    }
}