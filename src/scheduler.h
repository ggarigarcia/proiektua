#ifndef SCHEDULER
#define SCHEDULER

#include "prozesu-sortzaile.h"

/* DEFINEAK */
#define FCFS 0
#define SJF 1
#define RR 2

#define QUANTUM 5

/* METODOAK */
void hariak_eguneratu();
int haria_esleitu(pcb_ilara *ilara);
void haritik_atera(int hari_id, pcb *pcb, pcb_ilara *ilara); //dispatcherra, como tal
void hariak_pantailaratu();

int shortest_job_first(pcb_ilara *ilara);
int ilara_ordenatu(pcb_ilara *ilara); //schedulerra, como tal

void *timer_sched(void *arg);

#endif //SCHEDULER