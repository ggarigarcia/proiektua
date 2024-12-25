#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "prozesu-sortzaile.h"
#include "scheduler.h"
#include "main.h"

extern pthread_mutex_t mutex1;
extern pthread_cond_t cond1;
extern pthread_cond_t cond2;

extern uint done, abisu;
extern machine *makina;
extern int politika;

uint pcb_kont; //PCB-en id-ak esleitzeko
extern pcb_ilara *pcb_ilara_0, *pcb_ilara_1, *pcb_ilara_2, *pcb_ilara_finished;

pthread_mutex_t mutex_proc;
pthread_cond_t cond_proc1;
pthread_cond_t cond_proc2;

/* FUNTZIOAK */

pcb *pcb_sortu(int id)
{
    pcb *pcb_berri = malloc(sizeof(pcb));
    if (pcb_berri == NULL) return NULL;

    pcb_berri->info = malloc(sizeof(pcb_info));
    if (pcb_berri->info == NULL) {
        free(pcb_berri);
        return NULL;
    }

    pcb_berri->info->id = id;
    pcb_berri->info->egoera = NEW;
    pcb_berri->info->prioritatea = 0;
    pcb_berri->info->exek_denb = 50-pcb_kont;
    pcb_berri->info->quantum = QUANTUM;
    pcb_berri->hurrengoa = NULL;

    return pcb_berri;
}

void ilaran_gehitu(pcb_ilara *ilara, pcb *pcb)
{
    if (ilara->head == NULL) {
        ilara->head = pcb;
        ilara->tail = pcb;
    } else {
        ilara->tail->hurrengoa = pcb;
        ilara->tail = pcb;
    }

    pcb->hurrengoa = NULL;

    return;
}

pcb *ilaratik_atera(pcb_ilara *ilara)
{
    if(ilara->head == NULL)
    {
        return NULL;
    }

    pcb *pcb = ilara->head;
    
    if(ilara->head == ilara->tail)
    {
        ilara->tail = NULL;
    }
    ilara->head = ilara->head->hurrengoa;

    return pcb;
}

int ilara_hasieratu(pcb_ilara **ilara)
{
    *ilara = malloc(sizeof(pcb_ilara));
    if (*ilara == NULL) {
        return 1;
    }

    (*ilara)->head = NULL;
    (*ilara)->tail = NULL;

    return 0;
}

int ilara_ezabatu(pcb_ilara **ilara)
{
    //ilarako pcb guztiak ezabatu
    pcb *current = (*ilara)->head;
    pcb *next;
    while (current != NULL) {
        next = (pcb *) current->hurrengoa;
        free(current->info);
        free(current);
        current = next;
    }

    //ilara struct-a ezabatu
    free(*ilara);
    *ilara = NULL;
    
    return 0;
}

int ilara_pantailaratu(pcb_ilara *ilara)
{
    pcb *current = ilara->head;

    while(current != NULL)
    {
        printf(" - PCB %d, exek_denb: %d\n",current->info->id,current->info->exek_denb);
        current = (pcb *) current->hurrengoa;
    }

    return 0;
}

void timer_proc_amaitu()
{
    printf("\n\n-(PROC) Amaitu gabeko prozesuak:\n");
    ilara_pantailaratu(pcb_ilara_0);
    //ilara_pantailaratu(pcb_ilara_1);
    //ilara_pantailaratu(pcb_ilara_2);
    printf("\n-(PROC) Amaitutako prozesuak:\n");
    ilara_pantailaratu(pcb_ilara_finished);

    ilara_ezabatu(&pcb_ilara_0);
    ilara_ezabatu(&pcb_ilara_1);
    ilara_ezabatu(&pcb_ilara_2);
    ilara_ezabatu(&pcb_ilara_finished);

    return;
}

void *timer_proc(void *arg)
{
    timerArgs* t_args = (timerArgs*) arg;
    uint maiztasuna = t_args->maiztasuna;
    uint proc_tick = 0;

    pthread_mutex_lock(&mutex1);

    pcb_kont = 0;
    
    while(1)
    {
        done ++;

        if(abisu >= TTL) //amaitu
        {
            timer_proc_amaitu();
            pthread_mutex_unlock(&mutex1);

            return NULL;
        }
        
        proc_tick++;
        if(proc_tick == maiztasuna)
        {
            proc_tick = 0;

            pcb *pcb_berri = pcb_sortu(pcb_kont);
            pcb_kont++;
            ilaran_gehitu(pcb_ilara_0,pcb_berri);
            printf("-(PROC) PCB berria: id = %d, exek_denb = %d\n",pcb_berri->info->id,pcb_berri->info->exek_denb);  
        }
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2,&mutex1);
    }
}

