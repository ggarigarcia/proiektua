#ifndef SCHEDULER
#define SCHEDULER

#include "prozesu-sortzaile.h"

int ilara_ordenatu(pcb_ilara *ilara); //schedulerra, como tal

void *timer_sched(void *arg);

#endif //SCHEDULER