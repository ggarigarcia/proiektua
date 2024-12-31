#ifndef MAIN
#define MAIN

#include "scheduler.h"
#include "prozesu-sortzaile.h"

/* ------------------DATU EGITURAK------------------ */
#define TENP_KOP 2
#define TTL 20 // TTL <-> abisu

typedef unsigned int uint; // 0-tik 4294967295-erako balioak


typedef struct{
    //tlb
} mmu;


typedef struct{
    uint id;
    pcb *uneko_pcb;

    mmu mmu; // @logiko -> @fisiko
    int *ptbr; // uneko_pcb->mm->pgb
    //ir // uneko @logiko
    //pc //program counter
} hari;

/* MEMORIA FISIKOA */
#define MEMORIA_TAM (1 << 24) // 16MB
#define HITZ_TAM 4 // 4B 
#define ORRI_TAM (1 << 12) // 4KB

typedef struct memoria_fisikoa{

} memoria;

/* MAKINA */
typedef struct{
    uint total_hari_kop; //cpu_kop * core_kop * hari_kop
    uint *harimap; //harien bitmap
    hari *hariak; 
    //TODO memoria fisikoa
} machine;

/* TIMER */
typedef struct{
    uint maiztasuna;
    uint arg1;
} timerArgs;

/* ------------------METODOAK------------------ */
/* MAKINA */
int makina_hasieratu(uint cpu_kop, uint core_kop, uint hari_kop);
void makina_bukatu();

/* ERLOJU */
void *erloju(void *arg);

#endif //MAIN