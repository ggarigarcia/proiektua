# PROGRAMA EXEKUTATZEKO

- *Make run* idatzi terminalean, **proiektua** direktorioan konkretuki.
  - Agian *obj* eta *bin* karpeta sortu behar dira, github-en ez bait daude (make clean bidez binarioak ezabatzean, hutsak geratzen dira bi karpetak)
  - Programaren parametroak aldatzeko joan makefilera eta *run* atalean aldatu. Hauek dira parametroak:
    - clock_maizt sched_maizt proc_maizt cpu_kop core_kop hari_kop sched_politika ttl
  - Quantuma aldatzeko scheduler.h fitxateiko QUANTUM macro-a aldatu.
