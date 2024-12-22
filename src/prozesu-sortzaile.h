#ifndef PROZESU_SORTZAILE
#define PROZESU_SORTZAILE



/* EGITURAK */

/* PCB */

/* egoerak */
#define NEW 0
#define READY 1
#define RUNNING 2
#define WAITING 3
#define EXIT 4
typedef struct{
    int id;
    int egoera; //0: NEW, 1: READY, 2: RUNNING, 3: WAITING, 4: EXIT
    int prioritatea;
    int exek_denb;
} pcb_info;
typedef struct pcb{
    pcb_info *info;
    struct pcb *hurrengoa;
} pcb;

/* ILARA */

/* politikak */
#define FCFS 0
#define SJF 1
//#define RR 2
typedef struct{
    pcb *head;
    pcb *tail;
} pcb_ilara;

/* FUNTZIOAK */
int haria_esleitu(pcb *pcb); //TODO scheduler.h-ra pasa

/* pcb */
int pcb_gehitu(pcb *pcb, pcb_ilara *ilara);
int pcb_ezabatu(pcb *mypcb, pcb_ilara *ilara);
int pcb_mugitu(pcb *pcb, pcb_ilara *ilara1, pcb_ilara *ilara2);
int pcb_sortu(pcb **pcb_berri); //TODO egoera, prioritatea, exek_denb parametro gisa

/* ilara */
int ilara_ezabatu(pcb_ilara **ilara);
int ilara_hasieratu(pcb_ilara **ilara); //'**ilara' = struct-a eta punteroa(malloc/free/NULL) aldatzeko 
int ilara_erakutsi(pcb_ilara *ilara);

void *timer_proc(void *arg);

#endif //PROZESU_SORTZAILE