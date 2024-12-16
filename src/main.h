#define TENP_KOP 2

/* TIMER */
typedef struct{
    int maiztasuna;
} timerArg;

/* PCB */
typedef struct{
    int id;
    int egoera;
    int prioritatea;
} pcb_info;
typedef struct{
    pcb_info *info;
    struct pcb *hurrengoa;
} pcb;

typedef struct{
    pcb *head;
    pcb *tail;
    //pcb *unekoa;
} pcb_ilara;

/* MACHINE */
typedef struct{
    int id;
    pcb *uneko_pcb;
} hari;

typedef struct{
    int cpu_kop;
    int core_kop;
    int hari_kop;
    hari *hariak; 
} machine;

void *erloju(void *arg);
int makina_hasieratu(machine *makina,int cpu_kop, int core_kop, int hari_kop);
int makina_bukatu(machine *makina);

