#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <prozesu-sortzaile.h>
#include "scheduler.h"
#include "main.h"

extern pthread_mutex_t mutex1;
extern pthread_cond_t cond1;
extern pthread_cond_t cond2;
extern uint done, kont;
extern uint clock_done; //utzi erlojuari exekutatzen lehenengo
extern machine *makina;

uint pcb_kont; //TODO zertarako hau??
pcb_ilara *pcb_ilara_nagusia;
pcb_ilara *finished_ilara;

pthread_mutex_t mutex_proc;
pthread_cond_t cond_proc1;
pthread_cond_t cond_proc2;

/* FUNTZIOAK */
int haria_esleitu(pcb *pcb)
{
    //aurkitu lekua harimap-ean
    uint i = 0;
    while(i < makina->hari_kop)
    {
        if(makina->harimap[i] == 0) //libre
        {
            makina->hariak[i].uneko_pcb = pcb;
            makina->harimap[i] = 1;
            break;
        }
        i++;
    }

    if(i == makina->hari_kop) //beteta
    {
        printf("Hari guztiak okupatuta daude\n");
        return 1;
    } else{
        return 0;
    }
}

int pcb_gehitu(pcb_ilara *ilara, pcb *pcb)
{
    if (ilara->head == NULL) {
        ilara->head = pcb;
        ilara->tail = pcb;
    } else {
        ilara->tail->hurrengoa = (struct pcb *) pcb;
        ilara->tail = pcb;
    }

    //TODO CHANGE: ilarara sartu baina hari batera EZ
    haria_esleitu(pcb);

    return 0;
}

int pcb_ezabatu(pcb_ilara *ilara, pcb *mypcb)
{
    uint aurkitua = 0;

    pcb *uneko = malloc(sizeof(pcb)); pcb *aurreko = malloc(sizeof(pcb));
    if(uneko == NULL || aurreko == NULL){
        return 1; //malloc errorea
    }

    uneko = ilara->head;
    while(uneko != NULL)
    {

        if(uneko->info == mypcb->info->id) //aurkitua
        {
            //TODO ezabatu

        }
    }

    if(!aurkitua){
        printf("Errorea (2): ezin izan da PCB-a ezabatu. Ez da aurkitu");
        return 2;
    }

    return 0;
}

int pcb_sortu(pcb_ilara *ilara)
{
    pcb *pcb_berri = malloc(sizeof(pcb));
    if (pcb_berri == NULL) {
        perror("Malloc arazoa: pcb_berri sortzean");
        return -1;
    }

    pcb_berri->info = malloc(sizeof(pcb_info));
    if (pcb_berri->info == NULL) {
        perror("Malloc arazoa: pcb_berri->info sortzean");
        free(pcb_berri);
        return -1;
    }

    pcb_berri->info->id = pcb_kont;
    pcb_berri->info->egoera = 0; //NEW
    pcb_berri->info->prioritatea = 0;
    pcb_berri->info->exek_denb = (rand() % 10) + 1;
    pcb_berri->hurrengoa = NULL;

    pcb_gehitu(ilara,pcb_berri);

    printf("--prozesu berri bat sortu da: id = %d\n",pcb_kont);

    return 0;
}

int ezabatu_ilara(pcb_ilara *ilara)
{
    //free all elements of the linked list
    pcb *current = ilara->head;
    pcb *next;
    while (current != NULL) {
        next = (pcb *) current->hurrengoa;
        free(current->info);
        free(current);
        current = next;
    }

    free(ilara);
    ilara = NULL;
    
    return 0;
}

int erakutsi_ilara(pcb_ilara *ilara)
{
    pcb *current = ilara->head;

    printf("PCB ilara:\n");
    while(current != NULL)
    {
        printf("-PCB %d\n",current->info->id);
        current = (pcb *) current->hurrengoa;
    }

    return 0;
}

void *timer_proc(void *arg)
{
    pthread_mutex_lock(&mutex1);

    timerArgs* t_args = (timerArgs*) arg;
    uint maiztasuna = t_args->maiztasuna;
    uint proc_tick = 0;

    pcb_kont = 0;
    //ilarak hasieratu
    pcb_ilara_nagusia = malloc(sizeof(pcb_ilara));
    finished_ilara = malloc(sizeof(pcb_ilara));
    pcb_ilara_nagusia->head = NULL;
    pcb_ilara_nagusia->tail = NULL;

    while(1)
    {
        if(kont >= TTL)
        {
            pthread_mutex_unlock(&mutex1);
            erakutsi_ilara(pcb_ilara_nagusia);
            ezabatu_ilara(pcb_ilara_nagusia);
            ezabatu_ilara(finished_ilara);

            return NULL;
        }
        
        done ++;

        proc_tick++;
        if(proc_tick == maiztasuna)
        {
            proc_tick = 0;

            printf("Prozesu sortzaile\n");
            pcb_kont++;
            pcb_sortu(pcb_ilara_nagusia);
            
            
        }
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2,&mutex1);
    }
}

