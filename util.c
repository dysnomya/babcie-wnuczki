#include "main.h"
#include "util.h"
MPI_Datatype MPI_PAKIET_T;

struct tagNames_t
{
    const char *name;
    int tag;
} tagNames[] = {{"REQ_JAM", REQ_JAM}, {"REQ_JAR", REQ_JAR}, {"ACK", ACK}, {"RELEASE", RELEASE}};

const char const *tag2string(int tag)
{
    for (int i = 0; i < sizeof(tagNames) / sizeof(struct tagNames_t); i++)
    {
        if (tagNames[i].tag == tag)
            return tagNames[i].name;
    }
    return "<unknown>";
}
/* tworzy typ MPI_PAKIET_T
 */
void inicjuj_typ_pakietu()
{
    /* Stworzenie typu */
    /* Poniższe (aż do MPI_Type_commit) potrzebne tylko, jeżeli
       brzydzimy się czymś w rodzaju MPI_Send(&typ, sizeof(pakiet_t), MPI_BYTE....
    */
    /* sklejone z stackoverflow */
    int blocklengths[NITEMS] = {1, 1};
    // int blocklengths[NITEMS] = {1, 1, 1};
    MPI_Datatype typy[NITEMS] = {MPI_INT, MPI_INT};
    // MPI_Datatype typy[NITEMS] = {MPI_INT, MPI_INT, MPI_INT};

    MPI_Aint offsets[NITEMS];
    offsets[0] = offsetof(packet_t, ts);
    offsets[1] = offsetof(packet_t, src);

    MPI_Type_create_struct(NITEMS, blocklengths, offsets, typy, &MPI_PAKIET_T);

    MPI_Type_commit(&MPI_PAKIET_T);
}

/* opis patrz util.h */
void sendPacket(packet_t *pkt, int destination, int tag)
{
    int freepkt = 0;
    if (pkt == 0)
    {
        pkt = malloc(sizeof(packet_t));
        freepkt = 1;
    }

    // ustaw zegar
    changeClock(ts + 1);

    pkt->src = rank;
    pkt->ts = ts;


    MPI_Send(pkt, 1, MPI_PAKIET_T, destination, tag, MPI_COMM_WORLD);
    debug("Wysyłam %s do %d\n", tag2string(tag), destination);
    if (freepkt)
        free(pkt);
}

void sendPacketToBabcie(packet_t *pkt, int tag)
{
    for (int i = 0; i < BABCIE; i++)
    {
        if (i != rank)
        { // dont send to yourself!!!
            sendPacket(pkt, i, tag);
        }
    }
}

void sendPacketToWnuczki(packet_t *pkt, int tag)
{
    for (int i = BABCIE; i < size; i++)
    {
        if (i != rank)
        { // dont send to yourself!!!
            sendPacket(pkt, i, tag);
        }
    }
}

void sendPacketToEveryone(packet_t *pkt, int tag)
{

    sendPacketToBabcie(pkt, tag);
    sendPacketToWnuczki(pkt, tag);
}

void changeState(state_t newState)
{
    pthread_mutex_lock(&stateMut);
    // if (stan == InFinish)
    // {
    //     pthread_mutex_unlock(&stateMut);
    //     return;
    // }
    stan = newState;
    pthread_mutex_unlock(&stateMut);
}

void changeClock(int newClock)
{
    pthread_mutex_lock(&clockMut);
    ts = newClock;
    pthread_mutex_unlock(&clockMut);
}

void changeAckNum(int newAck)
{
    pthread_mutex_lock(&ackMut);
    ack_num = newAck;
    pthread_mutex_unlock(&ackMut);
}

