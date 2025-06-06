#include "main.h"
#include "watek_glowny.h"
#include "watek_komunikacyjny.h"

int rank, size;
state_t stan = IDLE;
pthread_t threadKom, threadMon;

// gotta say the goofiest locks i did in my life probably not correct thoXDD
pthread_mutex_t stateMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t clockMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t queueMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ackMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t critMut = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t waitMut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t waitCond = PTHREAD_COND_INITIALIZER;

int resource;

int ts = 0;
int ack_num;

PriorityQueue pq = {.size = 0};

void finalizuj()
{
    pthread_mutex_destroy(&stateMut);
    pthread_mutex_destroy(&clockMut);
    pthread_mutex_destroy(&queueMut);
    pthread_mutex_destroy(&ackMut);
    pthread_mutex_destroy(&critMut);
    pthread_mutex_destroy(&waitMut);
    /* Czekamy, aż wątek potomny się zakończy */
    println("czekam na wątek \"komunikacyjny\"\n");
    pthread_join(threadKom, NULL);
    MPI_Type_free(&MPI_PAKIET_T);
    MPI_Finalize();
}

void check_thread_support(int provided)
{
    printf("THREAD SUPPORT: chcemy %d. Co otrzymamy?\n", provided);
    switch (provided)
    {
    case MPI_THREAD_SINGLE:
        printf("Brak wsparcia dla wątków, kończę\n");
        /* Nie ma co, trzeba wychodzić */
        fprintf(stderr, "Brak wystarczającego wsparcia dla wątków - wychodzę!\n");
        MPI_Finalize();
        exit(-1);
        break;
    case MPI_THREAD_FUNNELED:
        printf("tylko te wątki, ktore wykonaly mpi_init_thread mogą wykonać wołania do biblioteki mpi\n");
        break;
    case MPI_THREAD_SERIALIZED:
        /* Potrzebne zamki wokół wywołań biblioteki MPI */
        printf("tylko jeden watek naraz może wykonać wołania do biblioteki MPI\n");
        break;
    case MPI_THREAD_MULTIPLE:
        printf("Pełne wsparcie dla wątków\n"); /* tego chcemy. Wszystkie inne powodują problemy */
        break;
    default:
        printf("Nikt nic nie wie\n");
    }
}

int main(int argc, char **argv)
{
    MPI_Status status;
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    check_thread_support(provided);
    srand(rank);
    inicjuj_typ_pakietu(); // tworzy typ pakietu
    packet_t pkt;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank < BABCIE)
        resource = START_JAR;
    else
        resource = START_JAM;
    pthread_create(&threadKom, NULL, startKomWatek, 0);

    mainLoop();

    finalizuj();
    return 0;
}
