#ifndef SCHEDULER
#define SCHEDULER

#include "prozesu-sortzaile.h"

int ordenatu_ilara(pcb_ilara *ilara, int politika); //scheduler

void *timer_sched(void *arg);

#endif //SCHEDULER