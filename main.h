#define TENP_KOP 2
#define CORE_KOP_MAX 1000

typedef struct{
    int maiztasuna;
} timerArg;

typedef struct{
    int id;
    pcb *hurrengoa;
} pcb;

typedef struct{
    pcb *head;
    pcb *tail;
    pcb *unekoa;
} pcb_ilara;

//TO DO: CPU, CORE, THREAD
typedef struct{
    int id;
    pcb *uneko_pcb; //pcb-a atzitzeko, gero aldatzen joan
} hari;

typedef struct{
    hari *hariak;
} core;

typedef struct{
    core *coreak;
} cpu;

typedef struct{
    cpu *cpuak;
} machine;