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


int pcb_gehitu(pcb_ilara *ilara, pcb *pcb)
{
    if (ilara->head == NULL) {
        ilara->head = pcb;
        ilara->tail = pcb;
    } else {
        ilara->tail->hurrengoa = (struct pcb *) pcb;
        ilara->tail = pcb;
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

/**
 * @brief PCB bat sortu eta ilaran sartu
 */
int pcb_sortu(pcb_ilara *ilara, int id)
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

    pcb_berri->info->id = id;
    pcb_berri->info->egoera = 0; //NEW
    pcb_berri->info->prioritatea = 0;
    pcb_berri->hurrengoa = NULL;

    pcb_gehitu(ilara,pcb_berri);

    printf("--prozesu berri bat sortu da: id = %d\n",id);

    return 0;
}

/**
 * @brief Ilara bateko elementu guztiak ezabatu (ilara barne)
 * @
 */
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

void *timer_proc(void *arg)
{
    pthread_mutex_lock(&mutex1);

    timerArg* t_arg = (timerArg*) arg;
    int maiztasuna = t_arg->maiztasuna;
    int proc_tick = 0;

    int pcb_kont = 0;
    
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
            pcb_sortu(pcb_ilara_nagusia,pcb_kont);
            
            
        }
        pthread_cond_signal(&cond1);
        
        pthread_cond_wait(&cond2,&mutex1);
    }
}

