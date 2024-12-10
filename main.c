/* INCLUDEAK */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "main.h"

/* ALDAGAI GLOBALAK */
pthread_mutex_t mutex1;
pthread_cond_t cond1,cond2;

int done = 0;

/* FUNTZIOAK */
void *erloju(void *arg)
{
    timerArg* t_arg = (timerArg*) arg;
    int maiztasuna = t_arg->maiztasuna;
    int erloju_tick = 0;
    int kont = 0;

    while(1)
    {
        erloju_tick++;
        //check hariak()
        if (erloju_tick == maiztasuna )
        {
            pthread_mutex_lock(&mutex1);

            erloju_tick = 0;

            printf("Abisu %d \n",kont);
            kont++;

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

void *timer_sched(void *arg)
{
    pthread_mutex_lock(&mutex1);

    timerArg* t_arg = (timerArg*) arg;
    int maiztasuna = t_arg->maiztasuna;
    int sched_tick = 0;

    while(1)
    {
        done++;
        
        sched_tick++;
        if(sched_tick == maiztasuna)
        {
            sched_tick = 0;

            printf("Scheduler\n");
        }
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2,&mutex1);
    }
}

void *timer_proc(void *arg)
{
    pthread_mutex_lock(&mutex1);

    timerArg* t_arg = (timerArg*) arg;
    int maiztasuna = t_arg->maiztasuna;
    int proc_tick = 0;

    while(1)
    {
        done ++;

        proc_tick++;
        if(proc_tick == maiztasuna)
        {
            proc_tick = 0;

            printf("Prozesu sortzaile\n");
            
        }
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2,&mutex1); 
    }
}

// void *scheduler(void *arg)
// {
//     //CPUan zegoen pcb kendu
//         //pcb-aren kontextua gorde
//         //TO DO: espezifikatu gehiago pcb-aren tratamendua

//     //hartu pcb HEAD (HEAD hurrengoari pasa)

//     //pcb-a exekutatu erlojuaren maiztasuna kontuan izanez. NOLA??



// }

/* void makina_hasieratu(machine makina, int cpu_kop, int core_kop, int hari_kop)
{

} */

/* MAIN */
int main(int argc, char *argv[])
{
    /* argumentu egiaztapena */
    if(argc < 4)
    {
        printf("%s <clock_maizt> <sched_maizt> <proc_maizt>\n",argv[0]);
        return 2;
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
 
    //int cpu_kop = atoi(argv[4]);
    //int core_kop = atoi(argv[5]);
    //int hari_kop = atoi(argv[6]);

    //machine makina;
    //makina_hasieratu(&makina,cpu_kop,core_kop,hari_kop);

    pthread_create(&p1,NULL,erloju,(void*)&argClock);
    pthread_create(&p1,NULL,timer_sched,(void*)&argT_sched);
    pthread_create(&p1,NULL,timer_proc,(void*)&argT_proc);
    //pthread_create(&p4,NULL,scheduler,(void*)&argsched)

    /* kernela amaitu */
    pthread_join(p1,NULL); printf("clock amaitua\n");
    pthread_join(p2,NULL); printf("timer_sched amaitua\n");
    pthread_join(p3,NULL); printf("timer_proc amaitua\n");
    //pthread_join(p4,NULL); printf("scheduler amaituta\n");

    pthread_mutex_destroy(&mutex1);
    pthread_cond_destroy(&cond1);
    pthread_cond_destroy(&cond2);

    printf("Sistema behar bezala amaitu da\n");

    return 0;
}