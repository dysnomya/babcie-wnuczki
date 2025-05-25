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
#define STATE_CHANGE_PROB 5
#define BABCIE 3 // ile procesów babci. Studentek będzie ile uruchomionych procesów - ile babci
#define START_JAM 3 // z jaką pulą konfitur zaczyna program
#define START_JAR 3 // z jaką pulą słoików zaczyna program

extern int rank;
extern int size;
typedef enum
{
    IDLE,
    WAIT,
    INSECTION
} state_t;

extern state_t stan;
extern pthread_t threadKom, threadMon;

extern pthread_mutex_t stateMut;
extern pthread_mutex_t clockMut;
extern pthread_mutex_t queueMut;
extern pthread_mutex_t ackMut;
extern pthread_mutex_t critMut;

extern int resource;

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

const char const *tag2string(int tag);
const char const *state2string(state_t state);


#ifdef DEBUG
// np: [4] [00000013] [IN  ] [1] <- [RANK] [CLOCK] [STAN] [RESOURCE]
#define debug(FORMAT,...) printf("%c[%d;%dm [%d] [%08d] [%s] [%d] " FORMAT "%c[%d;%dm\n",  27, (1+(rank/7))%2, 31+(6+rank)%7, rank, ts, state2string(stan), resource, ##__VA_ARGS__, 27,0,37);
#else
#define debug(...) ;
#endif

// makro println - to samo co debug, ale wyświetla się zawsze
#define println(FORMAT,...) printf("%c[%d;%dm [%d] [%08d] [%s] [%d] " FORMAT "%c[%d;%dm\n",  27, (1+(rank/7))%2, 31+(6+rank)%7, rank, ts, state2string(stan), resource, ##__VA_ARGS__, 27,0,37);



void changeState(state_t);

void incrementClock();

void changeClock(int packetClock);

void changeAckNum(int newAck);

int isPositionValid(int pid);

void changeResource(int num);

int canBabciaEnterCs();

int canWnuczkaEnterCs();

#endif