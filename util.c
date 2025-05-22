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

struct stateNames_t
{
    const char *name;
    state_t state;
} stateNames[] = {{"IDLE", IDLE}, {"WAIT", WAIT}, {"IN  ", INSECTION}};

const char const *state2string(state_t state)
{
    for (int i = 0; i < sizeof(stateNames) / sizeof(struct stateNames_t); i++)
    {
        if (stateNames[i].state == state)
            return stateNames[i].name;
    }
    return "<unk>";
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
void sendPacket(packet_t *pkt, int destination, int tag, int clock)
{
    int freepkt = 0;
    if (pkt == 0)
    {
        pkt = malloc(sizeof(packet_t));
        freepkt = 1;
    }

    pkt->src = rank;

    pkt->ts = clock;


    MPI_Send(pkt, 1, MPI_PAKIET_T, destination, tag, MPI_COMM_WORLD);
    debug("Wysyłam %s do %d", tag2string(tag), destination);
    if (freepkt)
        free(pkt);
}

void sendPacketToBabcie(packet_t *pkt, int tag, int clock)
{
    for (int i = 0; i < BABCIE; i++)
    {
        if (i != rank)
        { // dont send to yourself!!!
            sendPacket(pkt, i, tag, clock);
        }
    }
}

void sendPacketToWnuczki(packet_t *pkt, int tag, int clock)
{
    for (int i = BABCIE; i < size; i++)
    {
        if (i != rank)
        { // dont send to yourself!!!
            sendPacket(pkt, i, tag, clock);
        }
    }
}

void sendPacketToEveryone(packet_t *pkt, int tag, int clock)
{

    sendPacketToBabcie(pkt, tag, clock);
    sendPacketToWnuczki(pkt, tag, clock);
}

void changeState(state_t newState)
{
    pthread_mutex_lock(&stateMut);
    stan = newState;
    pthread_mutex_unlock(&stateMut);
}

void incrementClock()
{
    pthread_mutex_lock(&clockMut);
    ts++;
    pthread_mutex_unlock(&clockMut);
}

void changeClock(int packetClock)
{
    pthread_mutex_lock(&clockMut);
    ts = MAX(ts, packetClock) + 1;
    pthread_mutex_unlock(&clockMut);

}

void changeAckNum(int increment)
{
    pthread_mutex_lock(&ackMut);
    if (increment)
    {
        ack_num++;
    } else {
        ack_num = 0;
    }
    pthread_mutex_unlock(&ackMut);
}

void changeResource(int num)
{
    pthread_mutex_lock(&critMut);
    resource = resource + num;
    pthread_mutex_unlock(&critMut);
}