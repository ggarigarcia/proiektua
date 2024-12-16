/* INCLUDEAK */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "main.h"
#include "scheduler.h"
#include "prozesu-sortzaile.h"

/* ALDAGAI GLOBALAK */
pthread_mutex_t mutex1;
pthread_cond_t cond1,cond2;

int done;
int kont; //test-ak egiteko

/* FUNTZIOAK */
void *erloju(void *arg)
{
    timerArg* t_arg = (timerArg*) arg;
    int maiztasuna = t_arg->maiztasuna;
    int erloju_tick = 0;
    kont = 0;

    while(1)
    {
        erloju_tick++;
        //check hariak() //pcb-ei exekuzio denbora kendu
        if (erloju_tick == maiztasuna)
        {
            pthread_mutex_lock(&mutex1);

            erloju_tick = 0;

            printf("Abisu %d \n",kont);
            if(kont == 10){
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

            kont++;
        }
    }
}

int makina_hasieratu(machine *makina,int cpu_kop, int core_kop, int hari_kop)
{
    makina->cpu_kop = cpu_kop;
    makina->core_kop = core_kop;
    makina->hari_kop = hari_kop;
    makina->hariak = malloc(cpu_kop * core_kop * hari_kop * sizeof(hari));

    //hari bakoitza hasieratu
    int hari_total = cpu_kop * core_kop * hari_kop;
    for(int i = 0; i < hari_total; i++)
    {
        makina->hariak[i].id = i;
        makina->hariak[i].uneko_pcb = NULL; 
    }

    return 0;
}

void makina_bukatu(machine *makina)
{
    if (makina->hariak != NULL) {
        free(makina->hariak);
        makina->hariak = NULL;
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

    /* kernela martxan jarri */
    printf("Sistema martxan jartzen...\n");

    pthread_mutex_init(&mutex1,NULL);
    pthread_cond_init(&cond1,NULL);
    pthread_cond_init(&cond2,NULL);

    pthread_t p1,p2,p3;
    timerArg argClock = {atoi(argv[1])};
    timerArg argT_sched = {atoi(argv[2])};
    timerArg argT_proc = {atoi(argv[3])};
    //timerArg argScheduler = {atoi(argv[n])}
 
    int cpu_kop = atoi(argv[4]);
    int core_kop = atoi(argv[5]);
    int hari_kop = atoi(argv[6]);

    machine makina;
    makina_hasieratu(&makina,cpu_kop,core_kop,hari_kop); //

    pthread_create(&p1,NULL,erloju,(void*)&argClock);
    pthread_create(&p2,NULL,timer_sched,(void*)&argT_sched);
    pthread_create(&p3,NULL,timer_proc,(void*)&argT_proc);
    //pthread_create(&p4,NULL,scheduler,(void*)&argsched)

    /* kernela amaitu */
    pthread_join(p1,NULL); printf("--clock amaitua\n");
    pthread_join(p2,NULL); printf("--timer_sched amaitua\n");
    pthread_join(p3,NULL); printf("--timer_proc amaitua\n");
    //pthread_join(p4,NULL); printf("scheduler amaituta\n");

    printf("Sistema itzaltzen...\n");
    makina_bukatu(&makina);

    pthread_mutex_destroy(&mutex1);
    pthread_cond_destroy(&cond1);
    pthread_cond_destroy(&cond2);

    printf("Sistema behar bezala amaitu da\n");

    return 0;
}