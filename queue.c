// #include "util.h"
#include "main.h"

int queueCompare(const packet_t *a, const packet_t *b)
{
    if (a->ts != b->ts)
        return a->ts - b->ts;

    return a->src - b->src;
}

void queueInsert(packet_t newPacket)
{
    pthread_mutex_lock(&queueMut);
    if (pq.size >= MAX_QUEUE)
    {
        pthread_mutex_unlock(&queueMut);
        return; 
    }

    int i = pq.size - 1;

    while (i >= 0 && queueCompare(&pq.queue[i], &newPacket) > 0)
    {
        pq.queue[i + 1] = pq.queue[i];
        i--;
    }

    pq.queue[i + 1] = newPacket;
    pq.size++;

    pthread_mutex_unlock(&queueMut);
}

void queueRemove(int pid)
{
    pthread_mutex_lock(&queueMut);

    int i = 0;
    while(i < pq.size && (pq.queue[i]).src != pid) {
        i++;
    }
    
    if (i == pq.size) {
        pthread_mutex_unlock(&queueMut);
        return; // escape in case no pid in queue
    }

    for (i; i < pq.size - 1; i++) {
        pq.queue[i] = pq.queue[i+1];
    }

    pq.size--;

    pthread_mutex_unlock(&queueMut);
}

int isPositionValid(int pid)
{
    pthread_mutex_lock(&queueMut);
    pthread_mutex_lock(&critMut);
    int i = 0;
    int iterSize;

    iterSize = MIN(pq.size, resource);
    //debug("%d", iterSize);

    while (i < iterSize)
    {   
        if ((pq.queue[i]).src == pid)
        {   
            pthread_mutex_unlock(&queueMut);
            pthread_mutex_unlock(&critMut);
            return 1;
        }
        i++;
    }
    pthread_mutex_unlock(&queueMut);
    pthread_mutex_unlock(&critMut);
    return 0;
}

void printQueue()
{
    pthread_mutex_lock(&queueMut);

    for (int i = 0; i < pq.size; i++) {
        debug("PQ [%d] ts=%d, src=%d", i, pq.queue[i].ts, pq.queue[i].src);
    }

    pthread_mutex_unlock(&queueMut);

}