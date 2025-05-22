#ifndef QUEUEH
#define QUEUEH

#include "main.h"

#define MAX_QUEUE 100 // maksymalny rozmiar kolejki procesów oczekujących - !!!! byle nie był większy niż max liczba babci lub studentek

typedef struct {
    packet_t queue[MAX_QUEUE];
    int size;
} PriorityQueue;

int queueCompare(const packet_t *a, const packet_t *b);

void queueInsert(packet_t newPacket);

void queueRemove(int pid);

void printQueue();

#endif