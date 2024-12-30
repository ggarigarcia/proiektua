#ifndef SCHEDULER
#define SCHEDULER

#include "prozesu-sortzaile.h"

/* ------------------DEFINEAK------------------ */
#define FCFS 0
#define SJF 1
#define RR 2 // RR FCFS

#define RR_MA_EST 3 // Maila Anitz, ESTATIKOA
#define RR_MA_DIN 4 // Maila Anitz, DINAMIKOA (degradazioa)


#define QUANTUM 5

/* ------------------METODOAK------------------ */
/* HARIAK */
void hariak_eguneratu();
void hariak_pantailaratu();

/* DISPATCHER */
int haria_esleitu();
void haritik_atera(int hari_id, pcb_ilara *ilara, int egoera);
void round_robin(int hari_id);

/* SCHEDULER */
int shortest_job_first(pcb_ilara *ilara);
void ilara_ordenatu(pcb_ilara *ilara); //ilara anitzek ez dute SJF erabiltzen asike bakarrik ilara_0 ordenatu beharko dugu

/* TIMER_SCHED */
void *timer_sched(void *arg);

#endif //SCHEDULER