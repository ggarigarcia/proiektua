#ifndef PROZESU_SORTZAILE
#define PROZESU_SORTZAILE

/* PCB EGOERAK */
#define NEW 0
#define READY 1
#define RUNNING 2
#define WAITING 3
#define EXIT 4

/* EGITURAK */
typedef struct{
    int id;
    int egoera;
    int prioritatea;
    int exek_denb;
} pcb_info;
typedef struct{
    pcb_info *info;
    struct pcb *hurrengoa;
} pcb;

typedef struct{
    pcb *head;
    pcb *tail;
    //pcb *unekoa;
    //int politika; //0: FIFO, 1: SJF, 2: RR
} pcb_ilara;

/* FUNTZIOAK */
int pcb_gehitu(pcb_ilara *ilara, pcb *pcb);
int pcb_ezabatu(pcb_ilara *ilara, pcb *pcb); //TODO
int haria_esleitu(pcb *pcb);
int pcb_sortu(pcb_ilara *ilara);
int erakutsi_ilara(pcb_ilara *ilara);
int ezabatu_ilara(pcb_ilara *ilara);
void *prozesu_sortzaile(void *arg);
void *timer_proc(void *arg);

#endif //PROZESU_SORTZAILE