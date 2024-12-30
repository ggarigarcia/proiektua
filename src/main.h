#ifndef MAIN
#define MAIN

#include "scheduler.h"
#include "prozesu-sortzaile.h"

/* ------------------DATU EGITURAK------------------ */
#define TENP_KOP 2
#define TTL 20 // TTL <-> abisu

typedef unsigned int uint; // 0-tik 4294967295-erako balioak

/* TIMER */
typedef struct{
    uint maiztasuna;
    uint arg1;
} timerArgs;

/* MACHINE */
typedef struct{
    uint id;
    pcb *uneko_pcb;
    //mmu
    //tlb
    //ptbr //pable table base register
    //ir //instruction register
    //pc //program counter
} hari;

#define HITZ_KOP 10
#define HITZ_LUZERA 10

typedef struct{
    
} mem_block;
typedef struct memoria_fisikoa{
    //24 biteko helbide busa
    //32 biteko datu-busa
    
} phys_mem;

typedef struct{
    uint total_hari_kop; //cpu_kop * core_kop * hari_kop
    uint *harimap; //harien bitmap
    hari *hariak; 
} machine;

/* ------------------METODOAK------------------ */
/* MAKINA */
int makina_hasieratu(uint cpu_kop, uint core_kop, uint hari_kop);
void makina_bukatu();

/* ERLOJU */
void *erloju(void *arg);

#endif //MAIN