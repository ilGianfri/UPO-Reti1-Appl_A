# Laboratorio per il corso di Reti 1 (A.A. 2018/19) - Applicazione A

## The pdf inside the repository contains all the requirements (in italian). 

Sviluppare in linguaggio C un’applicazione di rete, costituita da un programma server ed un
programma client, in cui il programma server calcola la distribuzione dei caratteri (diversi da spazi)
che compongono un testo fornito dal client, secondo le specifiche sotto indicate.
In particolare, sono da tenere in considerazione tutti i caratteri ASCII a 7-bit alfanumerici (classe
alnum), ovvero i caratteri alfabetici (classe alpha) e le cifre numeriche (classe digit). Le classi di
caratteri sono quelle definite dall’ANSI C Standard Library e incluse in ctype.h. Questo esclude
spazi, punteggiature, caratteri di controllo, e caratteri accentati. Per semplicità e uniformità si
suppone che il server operi secondo la locale di default “C” o “POSIX”.
