#ifndef SCHEDULER
#define SCHEDULER

#include "prozesu-sortzaile.h"

/* METODOAK */
void hariak_eguneratu();
int haria_esleitu(pcb_ilara *ilara);
void haritik_atera(int hari_id, pcb *pcb, pcb_ilara *ilara); //harian dagoen PCB-a ilara batera (finished/pcb_nagusi) gehitu

int shortest_job_first(pcb_ilara *ilara);
int ilara_ordenatu(pcb_ilara *ilara); //schedulerra, como tal

void *timer_sched(void *arg);

#endif //SCHEDULER