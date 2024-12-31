# KERNEL BATEN SIMULATZAILEA PROGRAMATZEN

**Egilea:** Gari Garcia Egiguren  
**Unibertsitatea:** EHU/UPV  
**Ikasgaia:** Sistema Eragileak (Konputagailuen Ingeniaritza)  
**Ikasturtea:** 2024/25  

## Aurkibidea

- [Sarrera](#sarrera)
- [Lehenengo zatia: sistemaren hari nagusia](#lehenengo-zatia-sistemaren-hari-nagusia)
- [Bigarrengo zatia: scheduler/dispatcher](#bigarren-zatia-schedulerdispatcher)
- [Hirugarren zatia: memoria kudeatzailea](#hirugarren-zatia-memoria-kudeatzailea)

## Sarrera

Proiektu honen helburua sistema eragile baten **kernela** simulatzean da.

Sistema eragile baten **kernela** oinarrizko baliabideak kudeatzen dituen softwarea da. Software honek **memoria**, **prozesuak** eta **gailuak** kontrolatzen ditu, eta aplikazioen eta hardwarearen arteko **komunikazioa** ere ahalbidetzen du. Kernel-ak sistema eragilearen **muina** osatzen du, eta aplikazioek edo erabiltzaileek kernelaren bidez interakzionatzen dute sistema eragilearekin.

## Lehenengo zatia: sistemaren hari nagusia

Proiektuaren lehenengo zatian sistemaren hari nagusia definitu eta sortzen da.  
Hari nagusia hurrengo funtzio eta egiturek osatzen dute:

### Funtzioak

- Sistemaren erlojua (erloju): maiztasuna finkatzen du
- Tenporizadoreak: erlojuarekin komunikatzen dira
  - Scheduler/dispatcher (timer_sched)
  - Prozesu sortzailea (timer_proc)

``erloju``, ``timer_sched`` eta ``timer_proc`` funtzioak *pthread.h* liburutegiaren bidez sortutako **hariak** dira. Hari hauek beraien artean **komunikatu** eta **sinkronizatzeko**, liburutegi berdinean dauden **baldintzazko mutexak** erabiliko ditugu.

### Egiturak

- Makina (machine)
  - Hariak
- PCB (Process Control Block)
  - PCB ilara

### Prozedura

1. Lehenik eta behin, ``makina`` egitura hasieratuko da, argumentu bidez jasotako cpu, core eta hari kantitatearekin. Nire kasuan, ``total_hari_kop`` aldagaiak aurreko hiru argumentuen biderketa gordetzen du. Hari kantitatea, beraz, ``total_hari_kop`` aldagaiak definitzen du, eta horren baitan sortuko dira hariak eta harien bitmap-a (harimap), *malloc* bidez.

2. ``erloju`` hariak *tick*-ak sortuko ditu uneoro. Tick kopurua argumentu bidez jasotako maiztasunaren berdina denean, ``erloju``-k **abisu** bat pantailaratuko du eta mutex-a **askatuko** du.  

3. Askatutako mutexa ``timer_proc`` edo ``timer_sched`` hariak hartuko du. Hari bakoitzak bere maiztasun propioa du (argumentu bidez jasotakoa), eta abisu kopurua maiztasun horretara iristean dagokiona exekutatuko du:
   - ``timer_proc``-ek PCB berri bat sortuko du, eta PCB ilarara gehituko du.
   - ``timer_sched``-ek oraingoz ez du ezer egingo.



## Bigarren zatia: scheduler/dispatcher

## Hirugarren zatia: memoria kudeatzailea
