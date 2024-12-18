/* INCLUDEAK */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "main.h"
#include "scheduler.h"
#include "prozesu-sortzaile.h"

/* ALDAGAI GLOBALAK */
extern pcb_ilara *pcb_ilara_nagusia;
extern pcb_ilara *finished_ilara;

pthread_mutex_t mutex1;
pthread_cond_t cond1,cond2;

uint done;
uint kont; //TTL-rekin lotuta

machine *makina;

/* FUNTZIOAK */
int hariak_eguneratu()
{
    uint *uneko_exek_denb = NULL;

    uint i = 0;
    while(i < makina->hari_kop) //harimap guztia zeharkatu
    {
        if(makina->harimap[i] == 1) //okupatuta
        {
            uneko_exek_denb = &(makina->hariak[i].uneko_pcb->info->exek_denb);
            if(*uneko_exek_denb > 0) //jarraitu exek_denb kentzen
            {
                (*uneko_exek_denb)--;
                
            } else{ //BOTA (dispatcher??)
                pcb_mugitu(makina->hariak[i].uneko_pcb,pcb_ilara_nagusia,finished_ilara);
                makina->hariak[i].uneko_pcb = NULL; //punteroa modifikatzen du, ez PCB-a
                makina->harimap[i] = 0;
                printf("WARNING: %d haria libratu egin da\n",i);
            }
        }
        i++;
    }  
    
    return 0;
}

int makina_hasieratu(uint cpu_kop, uint core_kop, uint hari_kop)
{
    makina = malloc(sizeof(machine));
    if (makina == NULL)
    {
        return 1; //malloc errorea
    }
    
    makina->cpu_kop = cpu_kop;
    makina->core_kop = core_kop;
    makina->hari_kop = hari_kop;
    makina->total_hari_kop = cpu_kop * core_kop * hari_kop;
    makina->harimap = malloc(makina->total_hari_kop * sizeof(uint)); //bitmap
    if(makina->harimap == NULL)
    {
        return 1; //malloc errorea
    }
    makina->hariak = malloc(cpu_kop * core_kop * hari_kop * sizeof(hari));
    if(makina->hariak == NULL)
    {
        return 1; //malloc errorea
    }

    //bitmapa + hari bakoitza hasieratu
    for(int i = 0; i < makina->total_hari_kop; i++)
    {
        makina->hariak[i].id = i;
        makina->hariak[i].uneko_pcb = NULL; 
        makina->harimap[i] = 0;
    }

    return 0;
}

int makina_bukatu()
{
    if (makina->hariak != NULL) {
        free(makina->hariak);
        makina->hariak = NULL;
    }

    free(makina);
    makina = NULL;

    return 0;
}

void *erloju(void *arg)
{    
    timerArgs* t_args = (timerArgs*) arg;
    uint maiztasuna = t_args->maiztasuna;
    uint erloju_tick = 0;
    kont = 0;

    while(1)
    {
        erloju_tick++;
        if(kont != 0){
            hariak_eguneratu(); //PCB-en exekuzio denbora gutxitu
        }
        if (erloju_tick == maiztasuna)
        {
            pthread_mutex_lock(&mutex1);
            erloju_tick = 0;

            kont++;
            printf("Abisu %d \n",kont);
            if(kont == TTL){ //KERNELA AMAITU
                pthread_mutex_unlock(&mutex1);
                return NULL;
            }
            
            while(done < TENP_KOP)
            {
                pthread_cond_wait(&cond1,&mutex1);
            }
            done = 0;
            pthread_cond_broadcast(&cond2);
            pthread_mutex_unlock(&mutex1);
        }
    }
}

/* MAIN */
int main(int argc, char *argv[])
{
    /* argumentu egiaztapena */
    if(argc < 7)
    {
        printf("%s <clock_maizt> <sched_maizt> <proc_maizt> <cpu_kop> <core_kop> <hari_kop>\n",argv[0]);
        return 1;
    }

    /* KERNELA MARTXAN JARRI */
    printf("Sistema martxan jartzen...\n");

    pthread_mutex_init(&mutex1,NULL);
    pthread_cond_init(&cond1,NULL);
    pthread_cond_init(&cond2,NULL);

    pthread_t p1,p2,p3; //p4 = scheduler, p5 = dispatcher?
    timerArgs argClock = {atoi(argv[1])};
    timerArgs argT_sched = {atoi(argv[2])};
    timerArgs argT_proc = {atoi(argv[3])};
    //timerArgs argScheduler = {atoi(argv[n])}
 
    uint cpu_kop = atoi(argv[4]);
    uint core_kop = atoi(argv[5]);
    uint hari_kop = atoi(argv[6]);

    if(makina_hasieratu(cpu_kop,core_kop,hari_kop) != 0)
    {
        printf("Errorea makina sortzean\n");
        return 1;
    }

    pthread_create(&p1,NULL,erloju,(void*)&argClock);
    pthread_create(&p2,NULL,timer_sched,(void*)&argT_sched);
    pthread_create(&p3,NULL,timer_proc,(void*)&argT_proc);
    //pthread_create(&p4,NULL,scheduler,(void*)&argsched)

    /* ----------------------------------------------- */

    /* KERNELA AMAITU */
    pthread_join(p1,NULL); printf("--clock amaitua\n");
    pthread_join(p2,NULL); printf("--timer_sched amaitua\n");
    pthread_join(p3,NULL); printf("--timer_proc amaitua\n");
    //pthread_join(p4,NULL); printf("scheduler amaituta\n");

    printf("Sistema itzaltzen...\n");
    makina_bukatu();

    pthread_mutex_destroy(&mutex1);
    pthread_cond_destroy(&cond1);
    pthread_cond_destroy(&cond2);

    printf("Sistema behar bezala amaitu da\n");

    return 0;
}