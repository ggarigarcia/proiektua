#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "prozesu-sortzaile.h"
#include "scheduler.h"
#include "main.h"

/* ------------------ALDAGAIAK------------------ */
extern pthread_mutex_t mutex1;
extern pthread_cond_t cond1, cond2;

extern machine *makina;
extern uint done, abisu;

extern int politika;
extern pcb_ilara *pcb_ilara_0, *pcb_ilara_1, *pcb_ilara_2, *pcb_ilara_finished;
//extern pcb_ilara *pcb_ilara_array[TOTAL_PCB_ILARA];

uint pcb_kont; //PCB-en id-ak esleitzeko

/* ------------------METODOAK------------------ */
/* PCB */
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
    pcb_berri->info->exek_denb = 50 - pcb_kont; //TODO aldatu
    pcb_berri->info->quantum = QUANTUM;

    pcb_berri->hurrengoa = NULL;

    return pcb_berri;
}

void ilaran_gehitu(pcb_ilara *ilara, pcb *pcb, int egoera)
{
    if (ilara->head == NULL) {
        ilara->head = pcb;
        ilara->tail = pcb;
    } else {
        ilara->tail->hurrengoa = pcb;
        ilara->tail = pcb;
    }

    pcb->info->egoera = egoera;
    pcb->hurrengoa = NULL;

    return;
}

void ilaretan_gehitu(int hari_id)
{
    pcb *nire_pcb = makina->hariak[hari_id].uneko_pcb; 
    int *prio = &(nire_pcb->info->prioritatea); 

    //pixkanakako degradazioa, quantuma aldatu gabe
    if(politika == RR_MA_DIN) (*prio)++;

    switch (*prio)
    {
        case 0:
            ilaran_gehitu(pcb_ilara_0, nire_pcb, READY);
            printf("PCB %d OUT ilara 0-ra\n", nire_pcb->info->id);
            break;
        case 1:
            ilaran_gehitu(pcb_ilara_1, nire_pcb, READY);
            printf("PCB %d OUT ilara 1-ra\n", nire_pcb->info->id);
            break;
        case 2:
            ilaran_gehitu(pcb_ilara_2, nire_pcb, READY);
            printf("PCB %d OUT ilara 2-ra\n", nire_pcb->info->id);
            break;
        default:
            break;
    }

    return;
}

pcb *ilaratik_atera(pcb_ilara *ilara) 
{
    if(ilara->head == NULL) return NULL;

    pcb *pcb = ilara->head;
    
    if(ilara->head == ilara->tail)
    {
        ilara->tail = NULL;
    }
    ilara->head = ilara->head->hurrengoa;

    return pcb;
}

pcb *ilaretatik_atera() 
{
    //if not RR erabili HALARE, 0 ilararekin jokatu (besteak hutsak)
    pcb *nire_pcb = NULL;

    nire_pcb = ilaratik_atera(pcb_ilara_0);
    if(nire_pcb == NULL) nire_pcb = ilaratik_atera(pcb_ilara_1);
    if(nire_pcb == NULL) nire_pcb = ilaratik_atera(pcb_ilara_2);

    return nire_pcb;
}

/* ILARA */
int ilara_hasieratu(pcb_ilara **ilara)
{
    *ilara = malloc(sizeof(pcb_ilara));
    if (*ilara == NULL) return 1;

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

void ilara_pantailaratu(pcb_ilara *ilara)
{
    pcb *current = ilara->head;

    while(current != NULL)
    {
        printf(" - PCB %d, exek_denb: %d\n",current->info->id,current->info->exek_denb);
        current = (pcb *) current->hurrengoa;
    }

    return;
}

/* ILARA_ARRAY */
 int ilarak_hasieratu()
{
    ilara_hasieratu(&pcb_ilara_0);
    ilara_hasieratu(&pcb_ilara_1);
    ilara_hasieratu(&pcb_ilara_2);
    ilara_hasieratu(&pcb_ilara_finished);

    return 0; //error check??
}

void ilarak_amaitu()
{
    ilara_ezabatu(&pcb_ilara_0);
    ilara_ezabatu(&pcb_ilara_1);
    ilara_ezabatu(&pcb_ilara_2);

    return;
}

void ilarak_pantailaratu()
{
    //finished ezik, beste ilara guztiak
    ilara_pantailaratu(pcb_ilara_0);
    ilara_pantailaratu(pcb_ilara_1);
    ilara_pantailaratu(pcb_ilara_2);

    return;
} 

/* TIMER_PROC */
void timer_proc_amaitu()
{
    printf("\n\n-(PROC) Amaitu gabeko prozesuak:\n");
    ilarak_pantailaratu();

    printf("\n-(PROC) Amaitutako prozesuak:\n");
    ilara_pantailaratu(pcb_ilara_finished);

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
        if(abisu >= TTL) //amaitu
        {
            timer_proc_amaitu();
            pthread_mutex_unlock(&mutex1);

            return NULL;
        }

        done ++;
        
        proc_tick++;
        if(proc_tick == maiztasuna)
        {
            proc_tick = 0;

            pcb *pcb_berri = pcb_sortu(pcb_kont);
            pcb_kont++;
            ilaran_gehitu(pcb_ilara_0,pcb_berri, NEW);
            printf("-(PROC) PCB berria: id = %d, exek_denb = %d\n", pcb_berri->info->id, pcb_berri->info->exek_denb);  
        }
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2,&mutex1);
    }
}

