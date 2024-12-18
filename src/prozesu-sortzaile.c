#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "prozesu-sortzaile.h"
#include "scheduler.h"
#include "main.h"

extern pthread_mutex_t mutex1;
extern pthread_cond_t cond1;
extern pthread_cond_t cond2;
extern uint done, kont;
extern machine *makina;

uint pcb_kont; //PCB-en id-ak esleitzeko
pcb_ilara *pcb_ilara_nagusia;
pcb_ilara *finished_ilara;

pthread_mutex_t mutex_proc;
pthread_cond_t cond_proc1;
pthread_cond_t cond_proc2;

/* FUNTZIOAK */
int haria_esleitu(pcb *pcb)
{
    //aurkitu lekua harimap-ean
    uint i = 0;
    while(i < makina->hari_kop)
    {
        if(makina->harimap[i] == 0) //libre
        {
            makina->hariak[i].uneko_pcb = pcb;
            makina->harimap[i] = 1;
            break;
        }
        i++;
    }

    if(i == makina->hari_kop) //beteta
    {
        printf("Hari guztiak okupatuta daude\n");
        return 1;
    } else{
        return 0;
    }
}

int pcb_gehitu(pcb *pcb, pcb_ilara *ilara)
{
    //add pcb to the end of the list
    if (ilara->head == NULL) {
        ilara->head = pcb;
        ilara->tail = pcb;
    } else {
        ilara->tail->hurrengoa = (struct pcb *) pcb; 
        ilara->tail = pcb;
    }

    //haria_esleitu(pcb);

    return 0;
}

/* 
//PCB-a guztiz ezabatu ilaratik eta memoriatik
int pcb_guztiz_ezabatu(pcb *mypcb, pcb_ilara *ilara)
{
    pcb *current = ilara->head;
    pcb *prev = NULL;
    while (current != NULL) {
        if (current->info->id == mypcb->info->id) { //AURKITUA
            if (prev == NULL) {  //head kasua
                ilara->head = (pcb *) current->hurrengoa;
            } else { //not head kasua
                prev->hurrengoa = current->hurrengoa;
            }
            if (current == ilara->tail) { //tail kasua
                ilara->tail = prev;
            }
            //ezabatu guztiz PCB-a
            free(current->info);
            free(current);
            return 0;
        }
        prev = current;
        current = (pcb *) current->hurrengoa;
    }

    return 1; //ez aurkitua
} */

/**
 * @brief PCB-a ilara batetik ezabatzen du, baina ez memoriatik
 */
int pcb_ezabatu(pcb *mypcb, pcb_ilara *ilara)
{
    pcb *current = ilara->head;
    pcb *prev = NULL;
    while (current != NULL) {
        if (current->info->id == mypcb->info->id) { //AURKITUA
            if (prev == NULL) {  //head kasua
                ilara->head = (pcb *) current->hurrengoa;
            } else { //not head kasua
                prev->hurrengoa = current->hurrengoa;
            }
            if (current == ilara->tail) { //tail kasua
                ilara->tail = prev;
            }
            //ez ezabatu memoriatik
            return 0;
        }
        prev = current;
        current = (pcb *) current->hurrengoa;
    }

    return 1; //ez aurkitua
}

//TODO CHECK. Se supone PCB-a geldi geratzen dela eta atzipenak aldatzen direla ezta??
int pcb_mugitu(pcb *pcb, pcb_ilara *ilara1, pcb_ilara *ilara2)
{
    //gehitu ilara2-ra
    pcb_gehitu(pcb,ilara2);
    //ezabatu ilara1-etik (baina ez memoriatik)
    pcb_ezabatu(pcb,ilara1);
}

int pcb_sortu(pcb **pcb_berri)
{
    //pcb
    *pcb_berri = malloc(sizeof(pcb));
    if (pcb_berri == NULL) {
        printf("Malloc arazoa: pcb_berri sortzean\n");
        return 1;
    }

    //pcb->info
    (*pcb_berri)->info = malloc(sizeof(pcb_info));
    if ((*pcb_berri)->info == NULL) {
        printf("Malloc arazoa: pcb_berri->info sortzean\n");
        free(*pcb_berri);
        return 1;
    }

    //gainontzeko aldagaiak
    (*pcb_berri)->info->id = pcb_kont;
    pcb_kont++;
    (*pcb_berri)->info->egoera = 0; //TODO parametro gisa
    (*pcb_berri)->info->prioritatea = 0; //TODO parametro gisa
    (*pcb_berri)->info->exek_denb = (rand() % 10) + 1; //TODO parametro gisa
    (*pcb_berri)->hurrengoa = NULL;

    printf("--prozesu berri bat sortu da: id = %d\n",pcb_kont);

    return 0;
}

int ilara_ezabatu(pcb_ilara **ilara)
{
    //elementu guztiak ezabatu
    pcb *current = (*ilara)->head;
    pcb *next;
    while (current != NULL) {
        next = (pcb *) current->hurrengoa;
        free(current->info);
        free(current);
        current = next;
    }

    free(*ilara);
    *ilara = NULL;
    
    return 0;
}

int ilara_hasieratu(pcb_ilara **ilara)
{
    *ilara = malloc(sizeof(pcb_ilara));
    if (*ilara == NULL) {
        printf("Malloc arazoa: ilara sortzean\n");
        return 1;
    }

    (*ilara)->head = NULL;
    (*ilara)->tail = NULL;

    return 0;
}

int ilara_erakutsi(pcb_ilara *ilara)
{
    pcb *current = ilara->head;

    printf("PCB ilara:\n");
    while(current != NULL)
    {
        printf("-PCB %d\n",current->info->id);
        current = (pcb *) current->hurrengoa;
    }

    return 0;
}

void *timer_proc(void *arg)
{
    pthread_mutex_lock(&mutex1);

    timerArgs* t_args = (timerArgs*) arg;
    uint maiztasuna = t_args->maiztasuna;
    uint proc_tick = 0;

    pcb_kont = 0;
    //TODO metodo bati deitu ilarak hasieratzeko
    ilara_hasieratu(&pcb_ilara_nagusia);
    ilara_hasieratu(&finished_ilara);

    while(1)
    {
        if(kont >= TTL)
        {
            pthread_mutex_unlock(&mutex1);
            printf("Amaitutako prozesuak:\n");
            ilara_erakutsi(finished_ilara);
            ilara_ezabatu(&pcb_ilara_nagusia);
            ilara_ezabatu(&finished_ilara);

            return NULL;
        }
        
        done ++;

        proc_tick++;
        if(proc_tick == maiztasuna)
        {
            proc_tick = 0;

            printf("Prozesu sortzaile\n");
            pcb *pcb_berri = NULL;
            pcb_sortu(&pcb_berri); //punteroaren HELBIDEA pasa, bestela balioak EZ dira aldatuko
            pcb_gehitu(pcb_berri,pcb_ilara_nagusia);
            haria_esleitu(pcb_berri);
            
            
        }
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2,&mutex1);
    }
}

