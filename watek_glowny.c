#include "main.h"
#include "watek_glowny.h"

void mainLoop()
{
    srandom(rank);
    int tag;

    while (1)
    {
        switch (stan)
        {
        case IDLE:
            int perc = random() % 100;

            if (perc < STATE_CHANGE_PROB)
            {
                debug("Zaczynam wysyłać");

                packet_t *pkt = malloc(sizeof(packet_t));

                changeAckNum(0); // before changing state so that they dont send ack earlier

                if (rank < BABCIE)
                {
                    tag = REQ_JAR;
                    pkt->ts = ts++;
                    sendPacketToBabcie(pkt, tag);
                }
                else
                {
                    tag = REQ_JAM;
                    sendPacketToWnuczki(pkt, tag);
                }
                
                changeState(WAIT);
                queueInsert(*pkt);
                printQueue();

                debug("Skończyłam wysyłać i zmieniam stan na WAIT");
            }

            break;

        case WAIT:
            
        default:
            break;
        }

        sleep(SEC_IN_STATE);
    }
}
