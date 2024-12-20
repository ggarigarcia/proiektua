#ifndef MAIN
#define MAIN

#include "scheduler.h"
#include "prozesu-sortzaile.h"

#define TENP_KOP 2
#define TTL 10 //kernela amaitzeko abisu kopurua

typedef unsigned int uint; //0-tik 4294967295-erako balioak

/* TIMER */
typedef struct{
    uint maiztasuna;
} timerArgs;

/* MACHINE */
typedef struct{
    uint id;
    pcb *uneko_pcb;
} hari;

typedef struct{
    uint cpu_kop; //argv[4]
    uint core_kop; //argv[5]
    uint hari_kop; //argv[6]
    /* --- */
    uint total_hari_kop; //cpu_kop * core_kop * hari_kop
    uint *harimap; //harien bitmapa
    hari *hariak; 
} machine;


int hariak_eguneratu(); //TODO scheduler.h??
int makina_hasieratu(uint cpu_kop, uint core_kop, uint hari_kop);
int makina_bukatu();
void *erloju(void *arg);

#endif //MAIN