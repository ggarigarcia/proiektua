#ifndef PROZESU_SORTZAILE
#define PROZESU_SORTZAILE

int pcb_gehitu(pcb_ilara *ilara, pcb *pcb);
int pcb_sortu(pcb_ilara *ilara);
int erakutsi_ilara(pcb_ilara *ilara);
int ezabatu_ilara(pcb_ilara *ilara);
void *prozesu_sortzaile(void *arg);
void *timer_proc(void *arg);

#endif //PROZESU_SORTZAILE