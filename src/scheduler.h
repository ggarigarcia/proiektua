#ifndef SCHEDULER
#define SCHEDULER

#include "prozesu-sortzaile.h"

/* DEFINEAK */
/* politikak */
#define FCFS 0
#define SJF 1
#define RR_FCFS 2
#define RR_SJF 3


#define QUANTUM 5

/* METODOAK */
void hariak_eguneratu();
int haria_esleitu(pcb_ilara *ilara);
void haritik_atera(int hari_id, pcb_ilara *ilara, int egoera); //dispatcherra, como tal
void hariak_pantailaratu();

void round_robin(int hari_id);
int shortest_job_first(pcb_ilara *ilara);
int ilara_ordenatu(pcb_ilara *ilara); //schedulerra, como tal

void *timer_sched(void *arg);

#endif //SCHEDULER