#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "prozesu-sortzaile.h"
#include "scheduler.h"
#include "main.h"

/* ------------------ALDAGAIAK------------------ */
extern pthread_mutex_t mutex1;
extern pthread_cond_t cond1, cond2;

extern machine *makina;
extern uint done, abisu, ttl;

extern int politika;
extern pcb_ilara *pcb_ilara_0, *pcb_ilara_1, *pcb_ilara_2, *pcb_ilara_finished;

uint pcb_kont, proc_m_max; //PCB-en id-ak, maiztasun maximoa

/* ------------------METODOAK------------------ */
/* PCB */
pcb *pcb_sortu()
{
    //pcb como tal
    pcb *pcb_berri = malloc(sizeof(pcb));
    if (pcb_berri == NULL) return NULL;

    //pcb info
    pcb_berri->info = malloc(sizeof(pcb_info));
    if (pcb_berri->info == NULL) {free(pcb_berri); return NULL;}

    pcb_berri->info->id = pcb_kont;
    pcb_kont++;
    pcb_berri->info->egoera = NEW;
    
    if(politika == RR_MA_EST) //SOILIK ESTATIKOA. DINAMIKOAN DENAK 0-tik HASI
        pcb_berri->info->prioritatea = (rand() % 2);
    else
        pcb_berri->info->prioritatea = 0;

    pcb_berri->info->exek_denb = (rand() % proc_m_max) + 1;
    pcb_berri->info->quantum = QUANTUM;

    //pcb memory management
    pcb_berri->mm = malloc(sizeof(mm));
    if (pcb_berri->mm == NULL) {free(pcb_berri->info); free(pcb_berri); return NULL;}

    //TODO pcb-aren orri taula sortu memoria FISIKOAN (kernel zatian)
    pcb_berri->mm->pgb = NULL;
    //TODO datuak eta kodea fitxategi batetik irakurri eta PCB-an esleitu
    pcb_berri->mm->code = NULL;
    pcb_berri->mm->data = NULL;

    pcb_berri->hurrengoa = NULL;

    return pcb_berri;
}

void ilaran_gehitu(pcb_ilara *ilara, pcb *pcb, int egoera)
{
    if (ilara->head == NULL) {
        ilara->head = pcb;
        ilara->tail = pcb;
    } else {
        ilara->tail->hurrengoa = pcb;
        ilara->tail = pcb;
    }

    pcb->info->egoera = egoera;
    pcb->hurrengoa = NULL;

    return;
}

void ilaretan_gehitu(int hari_id)
{
    pcb *nire_pcb = makina->hariak[hari_id].uneko_pcb; 
    int *prio = &(nire_pcb->info->prioritatea);
    int *quantum = &(makina->hariak[hari_id].uneko_pcb->info->quantum);

    *quantum = QUANTUM; //quantuma eguneratu

    if(politika == RR_MA_DIN){ //pixkanakako degradazioa, quantuma handituz
        *prio++; //prio eguneratu
        *quantum += *prio; 
    } 

    switch (*prio)
    {
        case 0:
            ilaran_gehitu(pcb_ilara_0, nire_pcb, READY); 
            printf("--(\033[34mDISP\033[0m) %d Haria: PCB %d \033[41mOUT\033[0m ilara 0-ra\n", hari_id, nire_pcb->info->id);
            break;
        case 1:
            ilaran_gehitu(pcb_ilara_1, nire_pcb, READY);
            printf("--(\033[34mDISP\033[0m) %d Haria: PCB %d \033[41mOUT\033[0m ilara 1-ra\n", hari_id, nire_pcb->info->id);
            break;
        default: // prio >= 2 --> pcb_ilara_2
            ilaran_gehitu(pcb_ilara_2, nire_pcb, READY);
            printf("--(\033[34mDISP\033[0m) %d Haria: PCB %d \033[41mOUT\033[0m ilara 2-ra\n", hari_id, nire_pcb->info->id);
            break;
    }

    return;
}

pcb *ilaratik_atera(pcb_ilara *ilara) 
{
    if(ilara->head == NULL) return NULL;

    pcb *pcb = ilara->head;
    
    if(ilara->head == ilara->tail)
        ilara->head = ilara->tail = NULL;
    else 
        ilara->head = ilara->head->hurrengoa;


    return pcb;
}

pcb *ilaretatik_atera() 
{
    //if not RR erabili HALARE, 0 ilararekin jokatu (besteak hutsak)
    pcb *nire_pcb = NULL;

    nire_pcb = ilaratik_atera(pcb_ilara_0);
    if(nire_pcb == NULL) nire_pcb = ilaratik_atera(pcb_ilara_1);
    if(nire_pcb == NULL) nire_pcb = ilaratik_atera(pcb_ilara_2);

    return nire_pcb;
}

/* ILARA */
int ilara_hasieratu(pcb_ilara **ilara)
{
    *ilara = malloc(sizeof(pcb_ilara));
    if (*ilara == NULL) return 1;

    (*ilara)->head = NULL;
    (*ilara)->tail = NULL;

    return 0;
}

int ilara_ezabatu(pcb_ilara **ilara)
{
    //ilarako pcb guztiak ezabatu
    pcb *current = (*ilara)->head;
    pcb *next;
    while (current != NULL) {
        next = (pcb *) current->hurrengoa;
        free(current->mm);
        free(current->info); 
        free(current);
        current = next;
    }

    //ilara struct-a ezabatu
    free(*ilara);
    *ilara = NULL;
    
    return 0;
}

void ilara_pantailaratu(pcb_ilara *ilara)
{
    pcb *current = ilara->head;

    while(current != NULL)
    {
        printf(" - PCB %d, exek_denb: %d\n",current->info->id,current->info->exek_denb);
        current = (pcb *) current->hurrengoa;
    }

    return;
}

/* ILARA_ARRAY */
 int ilarak_hasieratu()
{
    ilara_hasieratu(&pcb_ilara_0);
    ilara_hasieratu(&pcb_ilara_1);
    ilara_hasieratu(&pcb_ilara_2);
    ilara_hasieratu(&pcb_ilara_finished);

    return 0; //error check??
}

void ilarak_amaitu()
{
    ilara_ezabatu(&pcb_ilara_0);
    ilara_ezabatu(&pcb_ilara_1);
    ilara_ezabatu(&pcb_ilara_2);

    return;
}

void ilarak_pantailaratu()
{
    //finished ezik, beste ilara guztiak
    ilara_pantailaratu(pcb_ilara_0);
    ilara_pantailaratu(pcb_ilara_1);
    ilara_pantailaratu(pcb_ilara_2);

    return;
} 

/* TIMER_PROC */
void timer_proc_amaitu()
{
    printf("\n\033[33mAmaitutako prozesuak:\033[0m\n");
    ilara_pantailaratu(pcb_ilara_finished);

    printf("\n\033[33mAmaitu gabeko prozesuak:\033[0m\n");
    ilarak_pantailaratu();

    return;
}

void loader(){
    
    pcb *pcb_berri = pcb_sortu(pcb_kont);
    ilaran_gehitu(pcb_ilara_0,pcb_berri, NEW);
    printf("-(\033[33mPROC\033[0m) PCB berria: id = %d, exek_denb = %d\n", pcb_berri->info->id, pcb_berri->info->exek_denb);  

    return;
}

void *timer_proc(void *arg) //LOADER
{
    timerArgs* t_args = (timerArgs*) arg;
    uint maiztasuna = t_args->maiztasuna;
    proc_m_max = t_args->arg1;
    uint proc_tick = 0;
    srand(time(NULL)); // PCB id-entzat

    pthread_mutex_lock(&mutex1);

    pcb_kont = 0;
    
    while(1)
    {
        if(abisu >= ttl) //amaitu
        {
            timer_proc_amaitu();
            pthread_mutex_unlock(&mutex1);

            return NULL;
        }

        done ++;
        
        proc_tick++;
        if(proc_tick == maiztasuna)
        {
            proc_tick = 0;
            loader();
        }
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2,&mutex1);
    }
}

