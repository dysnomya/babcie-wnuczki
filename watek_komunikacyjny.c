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
        changeClock(pakiet.ts);

        // debug("Dostałem %s od %d", tag2string(status.MPI_TAG), pakiet.src);
        

        switch (status.MPI_TAG)
        {
        case REQ_JAM:   
        case REQ_JAR:
            incrementClock();
            sendPacket(pkt, pakiet.src, ACK, ts);

            queueInsert(pakiet);
            // printQueue();

            break;
        case ACK:
            changeAckNum(1);
            break;
        case RELEASE:
            if ((rank < BABCIE) == (pakiet.src < BABCIE)) // XNOR -> we both gotta be grannies or students
            {
                queueRemove(pakiet.src);
                changeResource(-1);
            }
            else
            {
                changeResource(1);
            }
            
            break;
        default:
            break;
        }
    }
}
