#ifndef UTILH
#define UTILH
#include "main.h"

/* typ pakietu */
typedef struct
{
    int ts; /* timestamp (zegar lamporta */
    int src;

    // int data; /* przykładowe pole z danymi; można zmienić nazwę na bardziej pasującą  I DONT THINK WE NEED THIS ONE*/ 
} packet_t;


/* packet_t ma trzy pola, więc NITEMS=3. Wykorzystane w inicjuj_typ_pakietu */
#define NITEMS 2

/* Typy wiadomości */
#define REQ_JAR 1
#define REQ_JAM 2
#define ACK     3
#define RELEASE 4

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

extern MPI_Datatype MPI_PAKIET_T;
void inicjuj_typ_pakietu();

/* wysyłanie pakietu, skrót: wskaźnik do pakietu (0 oznacza stwórz pusty pakiet), do kogo, z jakim typem */
void sendPacket(packet_t *pkt, int destination, int tag, int clock);

/* wysyłanie pakietu do babci, skrót: wskaźnik do pakietu (0 oznacza stwórz pusty pakiet), z jakim typem */
void sendPacketToBabcie(packet_t *pkt, int tag, int clock);

/* wysyłanie pakietu do studentek, skrót: wskaźnik do pakietu (0 oznacza stwórz pusty pakiet), z jakim typem */
void sendPacketToWnuczki(packet_t *pkt, int tag, int clock);

/* wysyłanie pakietu do wszystkich, skrót: wskaźnik do pakietu (0 oznacza stwórz pusty pakiet), z jakim typem */
void sendPacketToEveryone(packet_t *pkt, int tag, int clock);

#endif
