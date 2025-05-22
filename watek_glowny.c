#include "main.h"
#include "watek_glowny.h"

void mainLoop()
{
    srandom(rank);
    int tag;
    int perc;

    while (1)
    {
        switch (stan)
        {
        case IDLE:
            perc = random() % 100;

            if (perc < STATE_CHANGE_PROB)
            {
                debug("Zaczynam wysyłać");

                packet_t *pkt = malloc(sizeof(packet_t));

                changeAckNum(0); // before changing state so that they dont send ack earlier
                incrementClock();

                if (rank < BABCIE)
                {
                    tag = REQ_JAR;

                    sendPacketToBabcie(pkt, tag, ts);
                }
                else
                {
                    tag = REQ_JAM;

                    sendPacketToWnuczki(pkt, tag, ts);
                }

                queueInsert(*pkt);
                changeState(WAIT);
                printQueue();

                debug("Skończyłam wysyłać i zmieniam stan na WAIT");
            }

            break;

        case WAIT:

            if (rank < BABCIE)
            {
                if (canBabciaEnterCs())
                {
                    changeState(INSECTION);
                    debug("Wchodzę do sekcji krytycznej I CZAS ROBIĆ KONFITURE");
                }
            }
            else
            {
                if (canWnuczkaEnterCs())
                {
                    changeState(INSECTION);
                    debug("Wchodzę do sekcji krytycznej I CZAS JEŚĆ KONFITURE");
                    break;
                }
            }
            break;
        case INSECTION:
            perc = random() % 100;

            if (perc < STATE_CHANGE_PROB)
            {
                debug("Opuszczam sekcję i zaczynam wysyłać");
                changeState(IDLE);

                packet_t *pkt = malloc(sizeof(packet_t));

                incrementClock();

                tag = RELEASE;
                sendPacketToEveryone(pkt, tag, ts);

                debug("Skończyłam wysyłać");

                changeResource(-1);

                queueRemove(rank);
                printQueue();
            }
            break;
        default:
            break;
        }

        sleep(SEC_IN_STATE);
    }
}

int canBabciaEnterCs()
{
    pthread_mutex_lock(&ackMut);
    if (ack_num != BABCIE - 1)
    {
        pthread_mutex_unlock(&ackMut);
        return 0;
    }
    debug("MAM WSZYSTKIE ACK: %d", ack_num);

    pthread_mutex_unlock(&ackMut);

    if (!isPositionValid(rank))
        return 0;
        
    debug("JESTEM NA OKEJ POZYCJI MOGĘ WCHODZIĆ");

    return 1;
}

int canWnuczkaEnterCs()
{
    pthread_mutex_lock(&ackMut);
    if (ack_num != size - BABCIE - 1)
    {
        pthread_mutex_unlock(&ackMut);
        return 0;
    }
    debug("MAM WSZYSTKIE ACK: %d", ack_num);

    pthread_mutex_unlock(&ackMut);

    if (!isPositionValid(rank))
        return 0;

    debug("JESTEM NA OKEJ POZYCJI MOGĘ WCHODZIĆ");

    return 1;
}