#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "main.h"
#include "scheduler.h"
#include "prozesu-sortzaile.h"

/* ------------------ALDAGAIAK------------------ */
machine *makina;

int politika;
pcb_ilara *pcb_ilara_0, *pcb_ilara_1, *pcb_ilara_2, *pcb_ilara_finished; 

pthread_mutex_t mutex1;
pthread_cond_t cond1, cond2;

uint done, abisu, ttl; // abisu <-> ttl

/* ------------------METODOAK------------------ */
/* MAKINA */
int makina_hasieratu(uint cpu_kop, uint core_kop, uint hari_kop)
{
    makina = malloc(sizeof(machine));
    if (makina == NULL) return 1;
    
    makina->total_hari_kop = cpu_kop * core_kop * hari_kop;

    //memoria fisikoa
        //TODO nola implementatu zerbait al que se le meten cosas en diferentes sitios y diferentes tamaños??

    //hariak
    makina->hariak = malloc(makina->total_hari_kop * sizeof(hari));
    if(makina->hariak == NULL) return 1;

    //harimap
    makina->harimap = malloc(makina->total_hari_kop * sizeof(uint));
    if(makina->harimap == NULL) return 1;
    
    //hari bakoitza + harimap hasieratu
    for(int i = 0; i < makina->total_hari_kop; i++)
    {
        makina->hariak[i].id = i;
        makina->hariak[i].uneko_pcb = NULL; 
        makina->harimap[i] = 0;
    }

    return 0;
}

void makina_bukatu()
{
    if (makina->hariak != NULL) {
        free(makina->hariak);
        makina->hariak = NULL;
    }

    free(makina);
    makina = NULL;

    return;
}

/* ERLOJU */
void *erloju(void *arg)
{    
    pthread_mutex_lock(&mutex1);
    timerArgs* t_args = (timerArgs*) arg; 
    uint maiztasuna = t_args->maiztasuna;
    uint erloju_tick = 0;
    abisu = 0;

    while(1)
    {
        erloju_tick++;
        usleep(1e6/maiztasuna);
        if(abisu > 0) hariak_eguneratu();

        if (erloju_tick == maiztasuna)
        {
            //pthread_mutex_lock(&mutex1);
            erloju_tick = 0;

            abisu++;
            printf("\n\033[35m(MAIN) Abisu %d \033[0m\n",abisu);  
            if(abisu == ttl){ //KERNELA AMAITU
                printf("\n\033[45mAMAIERA\033[0m");
                pthread_mutex_unlock(&mutex1);
                return NULL;
            }
            
            while(done < TENP_KOP)
            {
                pthread_cond_wait(&cond1,&mutex1);
            }
            done = 0;
            pthread_cond_broadcast(&cond2);
            //pthread_mutex_unlock(&mutex1);
        }
    }
}

/* ------------------MAIN------------------ */
int main(int argc, char *argv[])
{
    /* argumentu egiaztapena */
    if(argc < 9) {printf("\033[31mERABILERA:\033[0m\n%s <clock_maizt> <sched_maizt> <proc_maizt> <cpu_kop> <core_kop> <hari_kop> <sched_politika> <ttl>\n\n", argv[0]); return 1;}

    /* KERNELA HASIERATU */
    printf("\n\033[35mSistema martxan jartzen...\033[0m\n");

    pthread_mutex_init(&mutex1,NULL);
    pthread_cond_init(&cond1,NULL);
    pthread_cond_init(&cond2,NULL);

    pthread_t p1,p2,p3;
    timerArgs argClock = {atoi(argv[1])};
    timerArgs argT_sched = {atoi(argv[2])};
    timerArgs argT_proc = {atoi(argv[3]), atoi(argv[8])};    
 
    uint cpu_kop = atoi(argv[4]);
    uint core_kop = atoi(argv[5]);
    uint hari_kop = atoi(argv[6]);
    politika = atoi(argv[7]);
    ttl = atoi(argv[8]);

    makina_hasieratu(cpu_kop,core_kop,hari_kop);
    ilarak_hasieratu();

    //printf(" CLOCK: %d \n SCHED: %d \n PROC: %d \n TOTAL_HARI_KOP %d \n POLITIKA %d \n\n QUANTUM %d \n", atoi(argv[1]),atoi(argv[2]),atoi(argv[3]),makina->total_hari_kop,politika,QUANTUM);

    pthread_create(&p1,NULL,erloju,(void*)&argClock);
    usleep(1000); //TODO beste modu bat bilatu
    pthread_create(&p2,NULL,timer_sched,(void*)&argT_sched);
    pthread_create(&p3,NULL,timer_proc,(void*)&argT_proc);

    /* ----------------------------------------------- */

    pthread_join(p1,NULL);
    pthread_join(p2,NULL); 
    pthread_join(p3,NULL); 

    /* ----------------------------------------------- */

    /* KERNELA AMAITU */
    printf("\n\n\033[35mSistema itzaltzen...\033[0m\n");
    makina_bukatu();
    ilarak_amaitu();

    pthread_mutex_destroy(&mutex1);
    pthread_cond_destroy(&cond1);
    pthread_cond_destroy(&cond2);

    printf("\033[35mSistema behar bezala amaitu da\033[0m\n\n");

    return 0;
}