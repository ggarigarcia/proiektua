# KERNEL BATEN SIMULATZAILEA PROGRAMATZEN

Gari Garcia Egiguren  
Sistema Eragileak  
2024-2025 ikasturtea  

## 1. Sistemaren oinarria

Zati honetan sistemaren hari nagusia sortu beharko dugu, dagokioen egiturekin:  

- Sistemaren erlojua + bi tenporizadore: scheduler eta prozesu sortzaile
- Machine egitura, **n** hariz osatuta (n = cpu x core x hari_kop)
- PCB eta PCB_ilara egiturak

Programaren funtzionamendua:  

1. Erlojuak sistema kontrolatuko duten tick-ak sortuko ditu
2. Tick maiztasun konkretu batekin abisu bat bidaliko die bi tenporizadoreei. Hauek dagokien maiztasunarekin hurrengoa egingo dute:  
    - Scheduler: oraingoz ezer
    - Prozesu-sortzaile: PCB berri bat sortu eta ilarara gehitu

## 2. Planifikatzailea (Scheduler/Dispatcher)

Zati honetan Schedulerra eta Dispatcherra sortuko dira:  

- Sortutako PCB berriak makinako harietan exekutatuko dira, argumentuz pasatako politika erabiliz:  
  - 0: First Come First Served
  - 1: Shortest Job First
  - 2: Round-Robin + Shortest Job First
- Amaitutako prozesuak ilara desberdin batera gehituko dira

## 3. Memoriaren kudeatzailea
