#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "scheduler.h"
#include "main.h"

extern pthread_mutex_t mutex1;
extern pthread_cond_t cond1;
extern pthread_cond_t cond2;
extern int done,kont;
//extern int clock_done; //utzi erlojuari exekutatzen lehenengo
extern machine *makina;

int pcb_kont;

pthread_mutex_t mutex_proc;
pthread_cond_t cond_proc1;
pthread_cond_t cond_proc2;

/* FUNTZIOAK */
int haria_esleitu(pcb *pcb)
{
    if(makina->hari_aktibo_kop <= makina->hari_kop) //lekua dago => gehitu
    {
        makina->hariak[makina->hari_aktibo_kop].uneko_pcb = pcb;
        makina->hari_aktibo_kop++;
    }
    
    return 0;
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

    haria_esleitu(pcb);

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
    /* while(clock_done == 1)
    {
        ; //utzi erlojuari exekutatzen lehenengo
    } */

    pthread_mutex_lock(&mutex1);

    timerArgs* t_args = (timerArgs*) arg;
    int maiztasuna = t_args->maiztasuna;
    int proc_tick = 0;

    pcb_kont = 0;
    //ilara nagusia hasieratu
    pcb_ilara *pcb_ilara_nagusia = malloc(sizeof(pcb_ilara));
    pcb_ilara_nagusia->head = NULL;
    pcb_ilara_nagusia->tail = NULL;

    while(1)
    {
        if(kont >= 10)
        {
            pthread_mutex_unlock(&mutex1);
            erakutsi_ilara(pcb_ilara_nagusia);
            ezabatu_ilara(pcb_ilara_nagusia);

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

