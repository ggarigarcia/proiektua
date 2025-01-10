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
  - [Funtzionamendua](#funtzionamendua)
- [Hirugarren zatia](#hirugarren-zatia-memoria-kudeatzailea)
  - [Egiturak](#egiturak)
  - [Funtzioak](#funtzioak)
  - [Funtzionamendua](#hirugarren-zatiaren-funtzionamendua)

## Sarrera

Proiektu honen helburua sistema eragile baten **kernela** simulatzea da.

Sistema eragile baten **kernela** oinarrizko baliabideak kudeatzen dituen softwarea da. Software honek **memoria**, **prozesuak** eta **gailuak** kontrolatzen ditu, eta aplikazioen eta hardwarearen arteko **komunikazioa** ahalbidetzen du. Kernel-ak sistema eragilearen **muina** osatzen du, eta bai aplikazioak bai erabiltzaileak kernelaren bidez komunikatzen dira sistema eragilearekin.

## Lehenengo zatia: sistemaren hari nagusia

Proiektuaren lehenengo zatian sistemaren **hari nagusia** definitu eta sortzen da. Sistemak hainbat osagai izango ditu, hala nola ***makina***, **erlojua** eta **tenporizadoreak**, denak dagokioen moduan sinkronizatuta.  

### Hari nagusian inplementatutako funtzioak

- **Sistemaren erlojua** (``erloju``): maiztasuna finkatzen du, abisuak bidaliz
- **Tenporizadoreak**: erlojuaren abisuak jaso eta dagokien programa exekutatzen dute  
  - Scheduler/dispatcher (``timer_sched``)
  - Prozesu sortzailea (``timer_proc``)

``erloju``, ``timer_sched`` eta ``timer_proc`` funtzioak *pthread.h* liburutegiaren bidez sortutako hari **errealak** dira. Hari hauek haien artean **komunikatzeko** eta **sinkronizatzeko**, liburutegi berean dauden **baldintzazko mutexak** erabiliko dira.

### Hari nagusian inplementatutako egiturak

- **Makina (machine):** hardwarea simulatzen duen egitura. Hurrengo egiturez osatua dago:
  - **Hariak:** makina-ren hariak. Bektore bat da, eta hari bakoitzak ID bat eta PCB erakusle bat dauka
  - **Harimap:** hariak libre dauden (0) ala ez (1) adierazten duen bektorea. Bitmap gisa jokatzen du
- **PCB (Process Control Block):** Egitura ilara estekatu bateko *nodo* batena da. Hurrengo aldagaiak ditu:
  - **ID:** identifikadorea, PCB bakoitzeko desberdina.
  - **Hurrengo_pcb:** ilara estekatu baten egitura jarraituz, hurrengo PCB-aren erakusle bat.
- **PCB ilara:** PCB-en ilara estekatu bat. Ilararen hasiera (head) eta bukaera (tail) gordetzen ditu.

### Prozedura orokorra

1. Lehenengo, ``makina`` egitura hasieratuko da, argumentu bidez jasotako **cpu**, **core** eta **hari** kantitatearekin. Nire kasuan, ``total_hari_kop`` aldagaiak aurreko hiru argumentuen biderketa gordetzen du, cpu eta core aldagaiak ez bait dira erabiliko. Hari kantitatea, beraz, ``total_hari_kop`` aldagaiak definitzen du, eta honen baitan sortuko da **hariak** bektorea, eta harien **bitmap-a** (harimap), biak *malloc* bidez.

2. ``erloju`` hariak *tick*-ak sortuko ditu uneoro (kontagailu bat eguneratuz). Hari honek **maiztasun** bat du (argumentu bidez jasotakoa), eta segundu batean zenbat tick egin behar dituen espezifikatzen du. Tick kopurua maiztasunaren berdina denean, ``erloju``-k abisu bat pantailaratuko du terminalean, eta mutex-a **askatuko** du, beste hariei "abisu" bat bidaliz.  

3. Askatutako mutexa ``timer_proc`` edo ``timer_sched`` hariak hartuko du. Hari horrek dagokion kodea exekutatu eta mutexa askatuko du, beste hariak blokeatuz ondoren mutexa. Modu honetan bi timerrak "eguneratuta" daudela ziurtatzen dugu.  
``timer`` hari bakoitzak bere maiztasun propioa du (argumentu bidez jasotakoa), eta abisu kopurua maiztasun horretara iristen denean dagokion kodea exekutatuko du:
   - ``timer_proc``-ek PCB berri bat sortuko du, eta PCB ilaran gehituko du (oraingo ilara bakarra).
   - ``timer_sched``-ek oraingoz ez du ezer egingo.

Programa goian definitu dudan bezala exekutatuko bagenu, ez litzateke inoiz bukatuko, hiru hariak 'while(1)' batean sartzen bait dira. Hori ekiditeko, *ttl* (*time to leave*) aldagaia definitzen dut: abisu kopurua *ttl*-ra iristean sortutako hiru hariak (``erloju`` eta bi ``timer``rak) amaitzen dira, sistema itzaliz eta programa amaituz.  

## Bigarren zatia: scheduler/dispatcher

Proiektuaren bigarren zatian schedulerra eta dispatcherra programatzen dira. Bi osagai hauek exekuzioaren inguruko kontrol handiago bat eskaintzen dute, eta beharrezkoak izango dira PCB-en antolakuntza zuzena bermatzeko.  

### Scheduler

Schedulerrak sistemako ilarak ordenatuko ditu, politika desberdinak erabiliz. *ilara_ordenatu* metodoa exekutatuko du tokatzen zaion aldiro. Funtzionamendua atalean noiz deitzen zaion ikusiko dugu.

### Dispatcher

Dispatcherra testuinguru aldaketaz arduratuko da, hau da, PCB-ak harira **sartzeaz** eta haritik **ateratzeaz** (*haria_esleitu* eta *haritik_atera* metodoak exekutatuz, hurrenez hurren).  

Dispatcherrak eragiketa desberdinak egingo ditu politikaren baitan, *haria_esleitu* eta *haritik_atera* funtzioak erabiliz. Politikak hurrengo atalean azalduko dira.

### Funtzionamendua

Aurretik sistemaren funtzionamendu orokorra azaldu dugun bezala, orain scheduler eta dispatcherraren funtzionamendua tokatzen da azaltzea.

---

Schedulerraren eta dispatcherrarn implementazioa dela eta, aldaketa batzuk egin behar dira sisteman.

Politika parametroa terminaletik argumentu bezala pasako da, hurrengo balio posibleak edukiz:

- 0: FCFS
- 1: SJF
- 2: RR (FCFS erabiliz)
- 3: Maila anitzeko RR (FCFS), degradaziorik gabe PCB prioritatean
- 4: Maila anitzeko RR (FCFS), degradazioarekin (quantuma handituz)

Horrez gain, PCB egiturak aldagai berriak izango ditu:

- **Prioritatea:** PCB-ak duen exekuzio-lehentasuna, defektuz 0 (altuena).
- **Exekuzio-denbora**: PCB-ak makinako hari batean iraungo duen denbora maximoa. Ausaz esleitzen da PCB-a sortzean
- **Quantum**: Round Robin (RR) politika erabiltzean, PCB-aren exekuzio-tartea. Sisteman definitua.
- **Egoera**: PCB-aren uneko egoera definitzen dute: NEW/READY/RUNNING/FINISHED.

Bi PCB ilara berri sortuko dira, PCB-ak izan ditzakeen prioritate desberdinentzat. Programa exekutatzen den aldiro hasieratzen badira ere, ilara hauek soilik maila anitzeko politikaren bat aukeratzen bada erabiliko dira.

Aldaketa nagusia ``erloju`` hariko *eguneratu_hariak* metodoan aurkitzen da. Metodo honek makinako harietan dauden PCB-ei exekuzio-denbora **dekrementatzen** die, exekuzioa simulatuz. Schedulerrak eta dispatcherrak metodo honekin batera egingo dute lan.

---

Schedulerra ``timer_sched``-ren maiztasuna betetzean edo PCB bat hari batera sartu baina lehen exekutatuko da, dagokion ilara ordenatuz *ilara_ordenatu* metodoa erabiliz. Hurrengo bi ordenatze modu inplementatu dira metodo honen barruan, *politika* parametroaren baitan:

- FCFS: ilara arrunt baten funtzionamendua, lehenengoa iristen dena lehenengoa izango da exekutatzen.
- SJF: ilara exekuzio-denboraren baitan ordenatzen da, txikienetik handienera.

Dispatcherraren aktibazioa bere bi metodoen baitan dago: *haria_esleitu* eta *haritik_atera*.  
*haria_esleitu* metodoari hurrengo kasuetan deituko zaio:

1. timer_sched-ren maiztasuna betetzean.
2. PCB bat bere haritik atera eta gero, PCB berri bat sar dadin.

Bestalde, *haritik_atera* metodoaren aktibazioa **politika** aldagaiak definituko du. Politikak **ez** badu Round Robin (**RR**) erabiltzen, dispatcherra soilik PCB-aren exekuzio denbora amaitzean aktibatuko da, 'FINISHED' egoera esleituz PCB-ari eta *pcb_finished* ilarara gehituz.  
Aldiz, politikak **RR** erabiltzen badu, uneko PCB-aren **quantum-a** zerora iristean aterako du PCB-a haritik, exekuzioko ilara batera sartuz (ondoren berriz ere exekuta dadin).  

PCB-a exekuzio ilarara sartu behar denean, politikak aukeratuko du zein ilarara esleituko zaion:

- Ilara bakarrareko politikan: *pcb_ilara_0*.
- Ilara anitzeko politikan:
  - Degradaziorik gabe: PCB-a sortzean esleitutako prioritateari dagokion ilara (0, 1 edo 2)
  - Degradazioarekin: testuinguru aldaketaro prioritatea jaitsiko zaio, quantuma handituz. PCB-a bere prioritatea **jaitsi eta gero** gehituko da PCB-a ilarara.

## Hirugarren zatia: memoria kudeatzailea

Zati honetan memoria kudeatzaile bat inplementatu da, dagokion aldaketak burutuz hainbat funtzio eta egituretan. Tamalez, ez da lortu behar den guztia inplementatzea eta hirugarren zatiko lehenengo zatian eskatzen dena bakarrik egin da, hau da, datu egitura berriak sortzea eta zaharrak aldatzea.  

### Egiturak

Hasteko, egitura berriak sortuko dira:

- **Memoria fisikoa:** 64KB-eko bektore bat

Eta beste egitura batzuk moldatu egin beharko dira:

- Hariek honako aldagaiak izango dituzte memoria kudeaketarako:
  - mmu (memory management unit)
    - tlb (translation lookaside buffer)
  - ptbr (pable table base register)
  - ir (instruction register)
  - pc (program counter)
- Memory management izeneko egitura sortuko da PCB egituraren barruan, hurrengo aldagaiekin:
  - pgb (orri taularen helbidea memoria fisikoan)
  - code (kodearen segmentuaren helbide birtuala)
  - data (datuen segmentuaren helbide birtuala)

### Funtzioak

Hurrengo funtzioak aldatuak izan dira:

- *Makina_hasieratu*
  - Memoria fisikoa sortu eta hasieratu
  - Harien aldagai berriak hasieratu
- *PCB_sortu*
  - PCB-ak behar dituen egitura berriak sortu eta hasieratu

### Hirugarren zatiaren funtzionamendua

Programa hasieran makina_hasieratu funtzioa exekutatuko da, memoria fisikoa bertan sortuz (hariez gain).

PCB berri bat sortzen den aldiro orri-taula bat sortuko da memoria fisikoan, eta fitxategi batetik datuak eta kodea esleituko dira memoria fisikoan.
