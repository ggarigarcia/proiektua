# KERNEL BATEN SIMULATZAILEA PROGRAMATZEN

**Gari Garcia Egiguren**  

- [ggarcia149@ikasle.ehu.eus](mailto:ggarcia149@ikasle.ehu.eus)  
- **GitHub:** [@ggarigarcia](https://github.com/ggarigarcia)  

EHU/UPV - Informatika Fakultatea  
Konputagailuen Ingeniaritza - Sistema Eragileak  
2024/25 ikasturtea  

## Aurkibidea

- [Sarrera](#sarrera)
- [Lehenengo zatia](#lehenengo-zatia-sistemaren-hari-nagusia)
  - [Funtzioak](#hari-nagusian-inplementatutako-funtzioak)
  - [Egiturak](#hari-nagusian-inplementatutako-egiturak)
  - [Prozedura orokorra](#prozedura-orokorra)
- [Bigarrengo zatia](#bigarren-zatia-schedulerdispatcher)  
  - [Scheduler](#scheduler)  
  - [Dispatcher](#dispatcher)  
  - [Terminaleko jakinarazpenak](#terminaleko-jakinarazpenak)
- [Hirugarren zatia](#hirugarren-zatia-memoria-kudeatzailea)

## Sarrera

Proiektu honen helburua sistema eragile baten **kernela** simulatzean da.

Sistema eragile baten **kernela** oinarrizko baliabideak kudeatzen dituen softwarea da. Software honek **memoria**, **prozesuak** eta **gailuak** kontrolatzen ditu, eta aplikazioen eta hardwarearen arteko **komunikazioa** ere ahalbidetzen du. Kernel-ak sistema eragilearen **muina** osatzen du, eta aplikazioek edo erabiltzaileek kernelaren bidez interakzionatzen dute sistema eragilearekin.

## Lehenengo zatia: sistemaren hari nagusia

Proiektuaren lehenengo zatian sistemaren hari nagusia definitu eta sortzen da. Sistemak hainbat osagai izango ditu, hala nola erlojua, tenporizadoreak eta makina, denak behar diren bezala sinkronizatuta.  

### Hari nagusian inplementatutako funtzioak

- Sistemaren erlojua (erloju): maiztasuna finkatzen du, abisuak bidaliz
- Tenporizadoreak: erlojuaren abisuak jaso eta dagokien programa exekutatzen dute  
  - Scheduler/dispatcher (timer_sched)
  - Prozesu sortzailea (timer_proc)

``erloju``, ``timer_sched`` eta ``timer_proc`` funtzioak *pthread.h* liburutegiaren bidez sortutako hari **errealak** dira. Hari hauek beraien artean **komunikatu** eta **sinkronizatzeko**, liburutegi berdinean dauden **baldintzazko mutexak** erabiliko dira.

### Hari nagusian inplementatutako egiturak

- **Makina (machine):** total_hari_kop + hariak + harimap
  - **Hariak:** ID + uneko_pcb duten harien bektorea
  - **Harimap:** hariak libre dauden (0) ala ez (1) adierazten duen bektorea
- **PCB (Process Control Block):** ID + egoera + prioritatea + exekuzio_denbora + quantuma. Ilara estekatu bateko nodo baten egitura du
  - **PCB ilara:** pcb_head + pcb_tail. Ilara estekatu bateko hasiera eta bukaera, alegia

### Prozedura orokorra

1. Lehenengo, ``makina`` egitura hasieratuko da, argumentu bidez jasotako **cpu**, **core** eta **hari** kantitatearekin. Nire kasuan, ``total_hari_kop`` aldagaiak aurreko hiru argumentuen biderketa gordetzen du. Hari kantitatea, beraz, ``total_hari_kop`` aldagaiak definitzen du, eta horren baitan sortuko da **hariak** bektorea, eta harien **bitmap-a** (harimap), biak *malloc* bidez.

2. ``erloju`` hariak *tick*-ak sortuko ditu uneoro (kontagailu bat eguneratuz). Hari honek **maiztasun** bat du (argumentu bidez jasotakoa), eta segundu batean zenbat tick egin behar dituen espezifikatzen du. Tick kopurua maiztasunaren berdina denean, ``erloju``-k mutex-a **askatuko** du, beste hariei "abisu" bat bidaliz.  

3. Askatutako mutexa ``timer_proc`` edo ``timer_sched`` hariak hartuko du. Hariak dagokion kodea exekutatu eta mutexa askatuko du, beste hariak blokeatuz ondoren mutexa. Modu honetan bi timerrak "eguneratuta" daudela ziurtatzen dugu.  
``Timer`` hari bakoitzak bere maiztasun propioa du (argumentu bidez jasotakoa), eta abisu kopurua maiztasun horretara iristen denean dagokion kodea exekutatuko du:
   - ``timer_proc``-ek PCB berri bat sortuko du, eta PCB ilaran gehituko du
   - ``timer_sched``-ek oraingoz ez du ezer egingo

Programa goian definitu dudan bezala exekutatuko bagenu, ez litzateke inoiz bukatuko, hiru hariak 'while(1)' batean sartzen bait dira. Hori ekiditeko, *ttl* (*time to leave*) aldagaia definitzen da: abisu kopurua *ttl*-ra iristean sortutako hariak (``erloju`` eta bi ``timer``rak) amaitzen dira, sistema benetan amaitzea lortuz.  

## Bigarren zatia: scheduler/dispatcher

Proiektuaren bigarren zatian schedulerra eta dispatcherra programatzen dira. Bi osagai hauek exekuzioaren inguruko kontrol handiago bat eskaintzen dute, eta beharrezkoak izango dira PCB-en antolakuntza zuzena bermatzeko.  

### Scheduler

Schedulerrak sistemako ilarak ordenatuko ditu, politika desberdinak erabiliz. Schedulerra ``timer_sched``-ren maiztasuna betetzean eta PCB bat hari batera sartu baina lehen exekutatuko da (*ilara_ordenatu* metodoa exekutatuz). Hurrengo bi politikak inplementatu dira metodo honentzat:

- FCFS: ilara arrunt baten funtzionamendua, lehenengoa iristen dena lehenengoa izango da exekutatzen.
- SJF: ilara exekuzio-denboraren baitan ordenatzen da, txikienetik handienera.

### Dispatcher

Dispatcherra testuinguru aldaketaz arduratuko da, hau da, PCB-ak harira **sartzeaz** eta haritik **ateratzeaz** (*haria esleitu* eta *haritik atera* metodoak exekutatuz, hurrenez hurren).  

Dispatcherrak aktibazio desberdinak dauzka **politika** aldagaiaren arabera. Aldagai hau **argumentu** bezala pasatzen da, eta hainbat aukera desberdin eskaintzen ditu sakatutako zenbakiaren arabera:

- 0: FCFS
- 1: SJF
- 2: RR (FCFS erabiliz)
- 3: Maila anitzeko RR (FCFS), degradaziorik gabe
- 4: Maila anitzeko RR (FCFS), degradazioarekin (quantuma handituz)

Dispatcherraren aktibaziora bueltatuz, politika aldagaiak **ez** badu Round Robin (**RR**) erabiltzen, dispatcherra soilik PCB-aren exekuzio denbora amaitzean aktibatuko da, PCB amaituen ilarara erantsiz bukatu berri den PCB hori.  
Aldiz, politikak RR erabiltzen badu, uneko PCB-aren **quantum-a** zerora iristean aterako da haritik, kasu honetan PCB ilara "arrunt" batera sartzeko (ondoren berriz ere exekuta dadin).  

PCB-a ilara "arrunt" batera sartu behar denean, politikak aukeratuko du PCB-a zein ilarara esleituko den:

- Ilara bakarrareko politikan: *pcb_ilara_0*-ra (defektuzkoa).
- Ilara anitzeko politikan:
  - Degradaziorik gabe: PCB-a sortzean esleitutako prioritateari dagokion ilara (0, 1 edo 2)
  - Degradazioarekin: testuinguru aldaketaro prioritatea jaitsiko zaio, quantuma handituz aldiro. Prioritatea **jaitsi eta gero** gehituko da PCB-a ilarara, ez alderantziz.

Azaldutako guztia biltzeko, dispatcherraren magia ``erloju`` harian tick bakoitzero exekutatzen den metodoan aurkitzen da:  

- *Eguneratu_hariak* metodoak makinako harietan dauden PCB-ei exekuzio-denbora **dekrementatzen** die, benetazko exekuzioa simulatuz. Dispatcherraren bi metodo nagusiak metodo honen barnean aurkitzen dira, PCB-a noiz kanporatu/sartu itxoiten, exekuzio-denbora eta quantuma dekrementatu ahala.

### Terminaleko jakinarazpenak

Bukatzeko, garrantzitsua iruditzen zait terminalean pantailaratzen diren jakinarazpen desberdinak aipatzea.

- Dispatcherra exekutatzen den bakoitzean, kanporatu edo sartu den PCB-aren eta esleituta duen hariaren id-ak pantailaratzen dira.  
  - PCB-a amaitu bada, "AMAITU DA" pantailaratuko du.
  - PCB-aren quantum-a amaitu bada, "OUT" pantailaratuko du (dagokion exekuzio-denborarekin), seguruenik ondoren "IN" batez jarraitua (PCB berri bat sartu delako harira).
- Posiblea da ere hari guztiak okupatuak egotea, eta beraz ``timer_sched``-ek *haria_esleitu* metodoari deitzean "ez dago haririk libre" pantailaratzea.

## Hirugarren zatia: memoria kudeatzailea

Zati honetan memoria kudeatzaile bat inplementatu da, dagokion aldaketak burutuz funtzio eta egituretan.  

### Egiturak

Hasteko, egitura berriak sortu dira:

- Memoria fisikoa

Eta beste egitura batzuk moldatu egin behar izan dira:

- PCB-a
- Hariak

### Funtzioak

- *Makina_hasieratu*
  - Memoria fisikoa sortu eta hasieratu
- *PCB_sortu*
  - PCB-ak behar dituen egitura berriak sortu eta hasieratu, gehienak memoria fisikoarekin lotura dute
