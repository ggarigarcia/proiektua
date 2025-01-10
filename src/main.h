#ifndef MAIN
#define MAIN

#include "scheduler.h"
#include "prozesu-sortzaile.h"

/* ------------------DATU EGITURAK------------------ */
#define TENP_KOP 2

typedef unsigned int uint; // 0-tik 4294967295-erako balioak

typedef struct{
    int *tlb;
} mmu;


typedef struct{
    uint id;
    pcb *uneko_pcb;

    mmu *mmu; // @logiko -> @fisiko
    int *ptbr; // uneko_pcb->mm->pgb
    int *ir; // uneko @logiko
    uint pc; //program counter
} hari;

/* MEMORIA FISIKOA */


/* MAKINA */
typedef struct{
    uint total_hari_kop; //cpu_kop * core_kop * hari_kop
    uint *harimap; //harien bitmap
    hari *hariak; 
    int *memoria_fisikoa;
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