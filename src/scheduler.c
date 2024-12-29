#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "scheduler.h"
#include "main.h"
#include "prozesu-sortzaile.h"

/* ------------------ALDAGAIAK------------------ */
extern pthread_mutex_t mutex1;
extern pthread_cond_t cond1, cond2;

extern machine *makina;
extern uint done, abisu;

extern int politika; 
extern pcb_ilara *pcb_ilara_0, *pcb_ilara_1, *pcb_ilara_2, *pcb_ilara_finished;

/* ------------------METODOAK------------------ */
/* HARIAK */
void hariak_eguneratu()
{
    uint *uneko_exek_denb = NULL;

    uint i = 0;
    while(i < makina->total_hari_kop) //harimap guztia zeharkatu
    {
        if(makina->harimap[i] == 1) //okupatuta
        {
            uneko_exek_denb = &(makina->hariak[i].uneko_pcb->info->exek_denb);
            if(*uneko_exek_denb > 0) // exekuzio-denbora kendu
            {
                (*uneko_exek_denb)--;

                if(*uneko_exek_denb != 0 && politika >= RR) round_robin(i); //ELSE: bukatu arte mantendu harian    
            }

            if(*uneko_exek_denb == 0) // DISPATCHER
            { 
                haritik_atera(i, pcb_ilara_finished, FINISHED);
                haria_esleitu();
            }

            uneko_exek_denb = NULL;
        }

        i++;
    }  
    
    return;
}

void hariak_pantailaratu()
{
    for(int i = 0; i < makina->total_hari_kop; i++)
    {
        if(makina->harimap[i] == 1) //okupatuta
        {
            printf(" - PCB: id = %d, exek_denb = %d\n", makina->hariak[i].uneko_pcb->info->id, makina->hariak[i].uneko_pcb->info->exek_denb);

            //PCB-a ezabatu
            free(makina->hariak[i].uneko_pcb->info);
            makina->hariak[i].uneko_pcb->info = NULL;
            free(makina->hariak[i].uneko_pcb);
            makina->hariak[i].uneko_pcb = NULL;
        }
    }

    return;
}

/* DISPATCHER */
int haria_esleitu()
{
    pcb *nire_pcb;

    int i = 0;
    while(i < makina->total_hari_kop)
    {
        if(makina->harimap[i] == 0) //aurkitua
        {
            nire_pcb = ilaretatik_atera(); //TODO ilara ordenatu nunbait, sartu ondoren/ atera baino lehen

            if(nire_pcb != NULL)
            {
                makina->hariak[i].uneko_pcb = nire_pcb;
                makina->harimap[i] = 1;
                nire_pcb->info->egoera = RUNNING;

                printf("--(\033[44mDISP\033[0m) %d Haria: PCB %d \033[42mIN\033[0m %d\n", i, nire_pcb->info->id, nire_pcb->info->exek_denb);
                return 0;
            } else{
                //printf("--(DISP) Warning: ez dago PCB-rik ilaran\n");
                return 1;
            }
        }

        i++;
    }

    if(i == makina->total_hari_kop)
    {
        printf("--(\033[44mDISP\033[0m) \033[31mWarning:\033[0m ez dago haririk libre\n");
        return 2; 
    }

    return 0;
}

void haritik_atera(int hari_id, pcb_ilara *ilara, int egoera)
{
    if(ilara == pcb_ilara_finished)
    {
        ilaran_gehitu(ilara, makina->hariak[hari_id].uneko_pcb, FINISHED);
    } else {
        ilaretan_gehitu(hari_id);
    }

    if(politika < RR) printf("--(\033[44mDISP\033[0m) %d Haria: PCB %d \033[41mOUT\033[0m %d\n", hari_id, makina->hariak[hari_id].uneko_pcb->info->id, makina->hariak[hari_id].uneko_pcb->info->exek_denb);

    makina->harimap[hari_id] = 0;
    makina->hariak[hari_id].uneko_pcb = NULL;

    return;
}

void round_robin(int hari_id)
{
    int *uneko_quantum = &(makina->hariak[hari_id].uneko_pcb->info->quantum); 

    (*uneko_quantum)--;

    if(*uneko_quantum == 0)
    {
        *uneko_quantum = QUANTUM; //quantum-a reiniziatu
        
        if(makina->hariak[hari_id].uneko_pcb->info->exek_denb != 0) // 0 bada beste metodoak kanporatuko du
        {
            haritik_atera(hari_id, NULL, READY);
            haria_esleitu(NULL);
        }
        
    }

    return;
}

/* SCHEDULER */
int shortest_job_first(pcb_ilara *ilara)
{
    if (ilara == NULL || ilara->head == NULL) return 1; 

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
                // Info aldatu
                pcb_info *temp = ptr1->info;
                ptr1->info = next->info;
                next->info = temp;
                swapped = 1;
            }
            ptr1 = next;
        }
        lptr = ptr1;
    } while (swapped);

    return 0;
}

void ilara_ordenatu(pcb_ilara *ilara)
{
    switch(politika)
    {
        case SJF:
        //case RR_SJF: 
            shortest_job_first(ilara);
            break;
        default: //FCFS, RR
            break;
    }

    return;
}

/* TIMER SCHED */
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
            printf("\n\033[34mHarietan geratu diren prozesuak:\033[0m\n");
            hariak_pantailaratu();

            pthread_mutex_unlock(&mutex1);
            return NULL;
        }

        done++;
        
        sched_tick++;
        if(sched_tick == maiztasuna)
        {
            sched_tick = 0;

            ilara_ordenatu(pcb_ilara_0);
            haria_esleitu();
        }
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2,&mutex1);
    }
}