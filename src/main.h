#ifndef MAIN
#define MAIN

#include "scheduler.h"
#include "prozesu-sortzaile.h"

#define TENP_KOP 2

/* TIMER */
typedef struct{
    int maiztasuna;
} timerArgs;

/* MACHINE */
typedef struct{
    int id;
    pcb *uneko_pcb;
} hari;

typedef struct{
    int cpu_kop;
    int core_kop;
    int hari_kop;
    /* --- */
    int hari_aktibo_kop;
    hari *hariak; 
} machine;


int hariak_eguneratu();
int makina_hasieratu(int cpu_kop, int core_kop, int hari_kop);
int makina_bukatu();
void *erloju(void *arg);

#endif //MAIN