#include "main.h"
#include "watek_komunikacyjny.h"

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
void *startKomWatek(void *ptr)
{
    MPI_Status status;
    int is_message = FALSE;
    packet_t pakiet;
    packet_t *pkt;
    /* Obrazuje pętlę odbierającą pakiety o różnych typach */
    while (1)
    {
        // debug("czekam na recv");
        MPI_Recv(&pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        changeClock(MAX(ts, pakiet.ts) + 1);

        debug("Dostałem pakiet od %d", pakiet.src);
        

        switch (status.MPI_TAG)
        {
        case REQ_JAM:            
        case REQ_JAR:
            sendPacket(pkt, pakiet.src, ACK);
            // TODO: dodaj do kolejki żądań
            break;
        case ACK:
        case RELEASE:
        // TODO: usuń z kolejki żądań
            break;
        default:
            break;
        }
    }
}
