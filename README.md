# KERNEL BATEN SIMULATZAILEA PROGRAMATZEN

**Gari Garcia Egiguren**  

- [ggarcia149@ikasle.ehu.eus](mailto:ggarcia149@ikasle.ehu.eus)  
- **GitHub:** [@ggarigarcia](https://github.com/ggarigarcia)  

EHU/UPV - Informatika Fakultatea  
Konputagailuen Ingeniaritza - Sistema Eragileak  
2024/25 ikasturtea  

***

## Aurkibidea

- [Sarrera](#sarrera)
- [Lehenengo zatia: sistemaren hari nagusia](#lehenengo-zatia-sistemaren-hari-nagusia)
- [Bigarrengo zatia: scheduler/dispatcher](#bigarren-zatia-schedulerdispatcher)
- [Hirugarren zatia: memoria kudeatzailea](#hirugarren-zatia-memoria-kudeatzailea)

## Sarrera

Proiektu honen helburua sistema eragile baten **kernela** simulatzean da.

Sistema eragile baten **kernela** oinarrizko baliabideak kudeatzen dituen softwarea da. Software honek **memoria**, **prozesuak** eta **gailuak** kontrolatzen ditu, eta aplikazioen eta hardwarearen arteko **komunikazioa** ere ahalbidetzen du. Kernel-ak sistema eragilearen **muina** osatzen du, eta aplikazioek edo erabiltzaileek kernelaren bidez interakzionatzen dute sistema eragilearekin.

## Lehenengo zatia: sistemaren hari nagusia

Proiektuaren lehenengo zatian sistemaren hari nagusia definitu eta sortzen da. Sistemak hainbat osagai izango ditu, hala nola erlojua, tenporizadoreak eta makina.  

### Hari nagusian inplementatutako funtzio eta egiturak

#### Funtzioak

- Sistemaren erlojua (erloju): maiztasuna finkatzen du
- Tenporizadoreak: erlojuarekin komunikatzen dira
  - Scheduler/dispatcher (timer_sched)
  - Prozesu sortzailea (timer_proc)

``erloju``, ``timer_sched`` eta ``timer_proc`` funtzioak *pthread.h* liburutegiaren bidez sortutako **hariak** dira. Hari hauek beraien artean **komunikatu** eta **sinkronizatzeko**, liburutegi berdinean dauden **baldintzazko mutexak** erabiliko dira.

#### Egiturak

- Makina (machine): total_hari_kop + hariak + harimap.
  - Hariak: id + uneko_pcb
- PCB (Process Control Block)
  - PCB ilara: pcb_head + pcb_tail.

### Prozedura

1. Lehenengo, ``makina`` egitura hasieratuko da, argumentu bidez jasotako **cpu**, **core** eta **hari** kantitatearekin. Nire kasuan, ``total_hari_kop`` aldagaiak aurreko hiru argumentuen biderketa gordetzen du. Hari kantitatea, beraz, ``total_hari_kop`` aldagaiak definitzen du, eta horren baitan sortuko dira **hariak** eta harien **bitmap-a** (harimap), *malloc* bidez.

2. ``erloju`` hariak *tick*-ak sortuko ditu uneoro. Hari honek **maiztasun** bat du (parametroz pasatakoa), eta segundu batean zenbat tick egin behar dituen espezifikatzen du. Tick kopurua maiztasunaren berdina denean, ``erloju``-k mutex-a **askatuko** du, beste hariei "abisu" bat bidaliz.  

3. Askatutako mutexa ``timer_proc`` edo ``timer_sched`` hariak hartuko du. Hari horrek dagokion kodea exekutatu eta mutexa askatuko du, beste hariak blokeatuz orain mutexa. Modu honetan bi timerrak "eguneratzen" direla ziurtatzen dugu.  
Timer hari bakoitzak bere maiztasun propioa du (argumentu bidez jasotakoa), eta abisu kopurua maiztasun horretara iristen denean dagokion kodea exekutatuko du:
   - ``timer_proc``-ek PCB berri bat sortuko du, eta PCB ilaran gehituko du.
   - ``timer_sched``-ek oraingoz ez du ezer egingo.  

Programa goian definitu dudan bezala exekutatuko bagenu, ez litzateke inoiz bukatuko, hiru hariak 'while(1)' batean sartzen bait dira.  
Hori ekiditeko, *TTL* izeneko **macro**  bat definitu dut, non abisu kopurua *TTL*-ra iristen denean hariak bukatu egingo dira, sistema benetan itzaliz. *TTL* gure itzaltzeko botoia dela pentsa genezake.  

## Bigarren zatia: scheduler/dispatcher

Proiektuaren bigarren zatian schedulerra eta dispatcherra programatzen dira. Bi osagai hauek exekuzioaren inguruko kontrol handiago bat eskaintzen dute.  

### Scheduler

Schedulerrak sistemako ilarak ordenatuko ditu, politika desberdinak erabiliz. Schedulerra ``timer_sched``-ren maiztasuna betetzean eta PCB bat hari batera sartu baina lehen exekutatuko da (*ilara_ordenatu* metodoa). Hurrengo politikak inplementatu dira:

- FCFS: ilara arrunt baten funtzionamendua, lehenengoa iristen dena lehenengoa izango da exekutatzen.
- SJF: ilara exekuzio-denboraren baitan ordenatuko da, txikienetik handienera.

### Dispatcher

Dispatcherra testuinguru aldaketaz arduratuko da, hau da, PCB-ak harira sartzeaz  eta haritik ateratzeaz (*haria esleitu* eta *haritik atera* metodoak).
Dispatcherra PCB baten exekuzio-denbora (edo quantuma, RR politika aukeratua izan bada) amaitzean  aktibatuko da.

Aipatzekoa da politika argumentu bezala pasatzen dela aipatzea. Hona hemen argumentu posible guztiak:

- 0: FCFS
- 1: SJF
- 2: RR (FCFS erabiliz)
- 3: Maila anitzeko RR (FCFS), degradazio gabea
- 4: Maila anitzeko RR (FCFS), degradazioarekin (quantuma handituz)

Honez gain, ``erloju`` harian tick bakoitzero makinako hari batean dagoen PCB-ari exekuzio-denbora dekrementatuko zaio (*eguneratu_hariak* metodoa).

- RR kasuan quantuma aztertuko dugu, eta horren baitan dispatcherrari deitu ala ez erabaki.
- RR erabiltzen **ez** den kasuan PCB-a makinako harian geratuko da, exekuzio-denbora agortzen den arte.

## Hirugarren zatia: memoria kudeatzailea
