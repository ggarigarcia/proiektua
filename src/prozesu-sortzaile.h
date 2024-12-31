#ifndef PROZESU_SORTZAILE
#define PROZESU_SORTZAILE

/* ------------------DATU EGITURAK------------------ */
/* PCB */
#define NEW 0
#define READY 1
#define RUNNING 2
#define FINISHED 3

typedef struct memory_management{
    int *pgb; //PCB-aren orri taua MEMORIA FISIKOAN
    int *code; //code zatiaren helbide BIRTUALA
    int *data; //data zatiaren helbide BIRTUALA
} mm;
typedef struct{
    int id;
    int egoera;
    int prioritatea;
    int exek_denb;
    int quantum;
} pcb_info;

typedef struct{
    pcb_info *info;
    mm *mm;
    struct pcb *hurrengoa;
} pcb;

/* ILARA */
typedef struct{
    pcb *head;
    pcb *tail;
} pcb_ilara;

/* ------------------METODOAK------------------ */
/* PCB */
pcb *pcb_sortu();
void ilaran_gehitu(pcb_ilara *ilara, pcb *pcb, int egoera);
void ilaretan_gehitu(int hari_id); //ilara anitzak
pcb *ilaratik_atera(pcb_ilara *ilara);
pcb *ilaretatik_atera(); //ilara anitzak

/* ILARA */
int ilara_hasieratu(pcb_ilara **ilara);
int ilara_ezabatu(pcb_ilara **ilara);
void ilara_pantailaratu(pcb_ilara *ilara);

/* ILARA_ARRAY */
int ilarak_hasieratu(); //main funtzioan
void ilarak_amaitu(); //main funtzioan
void ilarak_pantailaratu();

/* TIMER_PROC */
void timer_proc_amaitu();
void *timer_proc(void *arg);

#endif //PROZESU_SORTZAILE