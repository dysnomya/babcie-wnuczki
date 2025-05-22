#ifndef MAINH
#define MAINH
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "util.h"
#include "queue.h"

/* boolean */
#define TRUE 1
#define FALSE 0


// można zmieniać:
#define SEC_IN_STATE 1
#define STATE_CHANGE_PROB 10
#define BABCIE 4 // ile procesów babci. Studentek będzie ile uruchomionych procesów - ile babci
#define START_JAM 1 // z jaką pulą konfitur zaczyna program
#define START_JAR 1 // z jaką pulą słoików zaczyna program

extern int rank;
extern int size;
typedef enum
{
    IDLE,
    WAIT,
    INSECTION,
    WORKING
} state_t;
extern state_t stan;
extern pthread_t threadKom, threadMon;

extern pthread_mutex_t stateMut;
extern pthread_mutex_t clockMut;
extern pthread_mutex_t queueMut;
extern pthread_mutex_t ackMut;

extern int jar;
extern int jam;
extern int ts;
extern int ack_num;

extern PriorityQueue pq;

/* macro debug - działa jak printf, kiedy zdefiniowano
   DEBUG, kiedy DEBUG niezdefiniowane działa jak instrukcja pusta

   używa się dokładnie jak printfa, tyle, że dodaje kolorków i automatycznie
   wyświetla rank

   w związku z tym, zmienna "rank" musi istnieć.

   w printfie: definicja znaku specjalnego "%c[%d;%dm [%d]" escape[styl bold/normal;kolor [RANK]
                                           FORMAT:argumenty doklejone z wywołania debug poprzez __VA_ARGS__
                       "%c[%d;%dm"       wyczyszczenie atrybutów    27,0,37
                                            UWAGA:
                                                27 == kod ascii escape.
                                                Pierwsze %c[%d;%dm ( np 27[1;10m ) definiuje styl i kolor literek
                                                Drugie   %c[%d;%dm czyli 27[0;37m przywraca domyślne kolory i brak pogrubienia (bolda)
                                                ...  w definicji makra oznacza, że ma zmienną liczbę parametrów

*/

#ifdef DEBUG
#define debug(FORMAT,...) printf("%c[%d;%dm [%d] [%08d] " FORMAT "%c[%d;%dm\n",  27, (1+(rank/7))%2, 31+(6+rank)%7, rank, ts, ##__VA_ARGS__, 27,0,37);
#else
#define debug(...) ;
#endif

// makro println - to samo co debug, ale wyświetla się zawsze
#define println(FORMAT,...) printf("%c[%d;%dm [%d] [%08d] " FORMAT "%c[%d;%dm\n",  27, (1+(rank/7))%2, 31+(6+rank)%7, rank, ts, ##__VA_ARGS__, 27,0,37);

void changeState(state_t);

void changeClock(int newClock);

void changeAckNum(int newAck);

#endif