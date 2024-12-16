#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "scheduler.h"
#include "main.h"

extern pthread_mutex_t mutex1;
extern pthread_cond_t cond1;
extern pthread_cond_t cond2;
extern int done,kont;

int pcb_kont;

pthread_mutex_t mutex_proc;
pthread_cond_t cond_proc1;
pthread_cond_t cond_proc2;


int pcb_gehitu(pcb_ilara *ilara, pcb *pcb)
{
    if(ilara->head == NULL) //
    {
        ilara->head = &pcb;
        ilara->tail = &pcb;
    } else{ //bukaeran gehitu
        ilara->tail->hurrengoa = &pcb;
        ilara->tail = &pcb;
    }

    return 0;
}

int pcb_sortu(pcb_ilara *ilara)
{
    pcb *pcb = {pcb_kont,NULL};
    if(ilara->head == NULL)
    
    pcb_gehitu(&ilara,&pcb);


    printf("--prozesu berri bat\n");
    return 0;
}

void *timer_proc(void *arg)
{
    pthread_mutex_lock(&mutex1);

    timerArg* t_arg = (timerArg*) arg;
    int maiztasuna = t_arg->maiztasuna;
    int proc_tick = 0;

    pcb_kont = 0;
    //TODO check ez duela errorerik emango tamainarekin pcb-ak gehitzen joan ahala
    pcb_ilara *pcb_ilara_nagusia = malloc(sizeof(pcb_ilara));

    while(1)
    {
        if(kont >= 10)
        {
            pthread_mutex_unlock(&mutex1);
            free(&pcb_ilara_nagusia);

            return NULL;
        }
        
        done ++;

        proc_tick++;
        if(proc_tick == maiztasuna)
        {
            proc_tick = 0;

            printf("Prozesu sortzaile\n");
            prozesua_sortu(&pcb_ilara_nagusia);
            
            
        }
        pthread_cond_signal(&cond1);
        
        pthread_cond_wait(&cond2,&mutex1);
    }
}

