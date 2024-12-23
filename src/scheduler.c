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
extern pcb_ilara *pcb_ilara_nagusia, *finished_ilara;

int politika; //FCFS, SJF, RR

/* METODOAK */
void hariak_eguneratu()
{
    uint *uneko_exek_denb = NULL;

    uint i = 0;
    while(i < makina->total_hari_kop) //harimap guztia zeharkatu
    {
        if(makina->harimap[i] == 1) //okupatuta
        {
            uneko_exek_denb = &(makina->hariak[i].uneko_pcb->info->exek_denb);
            if(*uneko_exek_denb > 0) //jarraitu exek_denb kentzen
            {
                (*uneko_exek_denb)--;
                
            } else{ //dispatcher
                haritik_atera(i,makina->hariak[i].uneko_pcb,finished_ilara);
                haria_esleitu(pcb_ilara_nagusia);
            }
        }
        i++;
    }  
    
    return;
}

int haria_esleitu(pcb_ilara *ilara)
{
    int i = 0;
    while(i < makina->total_hari_kop)
    {
        if(makina->harimap[i] == 0) //haria libre dago
        {
            pcb *pcb = ilaratik_atera(ilara);
            if(pcb != NULL)
            {
                makina->hariak[i].uneko_pcb = pcb;
                makina->harimap[i] = 1;
                printf("-(SCHED) %d haria esleitu zaio PCB %d-ri\n",i,pcb->info->id);
                return 0;
            }
        }
        i++;
    }

    printf("(WARNING) Ez dago haririk libre\n");
    return 1; 
}

void haritik_atera(int hari_id, pcb *pcb, pcb_ilara *ilara)
{
    ilaran_gehitu(ilara,pcb);
    makina->harimap[hari_id] = 0;
    makina->hariak[hari_id].uneko_pcb = NULL;

    printf("-(SCHED) PCB %d, %d haritik atera da\n",pcb->info->id,hari_id);

    return;
}


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
            printf("(SCHED) Harietan geratu diren prozesuak:\n");
            //TODO print okupatuta dauden hariak
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
            haria_esleitu(pcb_ilara_nagusia);
        }
        pthread_cond_signal(&cond1);
        
        pthread_cond_wait(&cond2,&mutex1);
    }
}