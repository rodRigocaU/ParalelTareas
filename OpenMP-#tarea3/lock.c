#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

//CREEMOS LA COLA COMPARTIDA
int send_max = 10;
int thread_count;
struct queue *qq[6];
int done_sending = 0;

struct node_q
{
    int message;
    int src;
    struct node_q *next;
};

struct queue
{
    int enqueue, dequeue;

    struct node_q *tail;
    struct node_q *head;
    struct node_q *next_p;
};

struct queue *InitQ()
{
    struct queue *q_p = malloc(sizeof(struct queue));
    q_p->enqueue = q_p->dequeue = 0;
    q_p->head = NULL;
    q_p->tail = NULL;
    return q_p;
}

void Enqueue(struct queue *q, int dato, int src)
{
    struct node_q *ptr = malloc(sizeof(struct node_q));
    ptr->src = src;
    ptr->message = dato;
    ptr->next = NULL;

    //PRIMER CASO NO EXISTE UNA COLA PREVIA
    if (q->tail == NULL)
    {
        q->head = ptr;
        q->tail = ptr;
    }
    else
    {
        q->tail->next = ptr;
        q->tail = ptr;
    }

    q->enqueue++;
}

int Dequeue(struct queue *q, int *dato, int *src, int sr)
{
    if (q->head == NULL)
    {
        //printf("NO ELEMENTS!!");
        return 0;
    }
    else
    {

        struct node_q *ptrT;
        *dato = q->head->message;
        *src = q->head->src;
        ptrT = q->head;
        if (q->head == q->tail)
        {

            q->head = q->tail = NULL;
        }
        else
        {
            //q->head = q->head->next;
            q->head = ptrT->next;
        }
        free(ptrT);
        q->dequeue++;
        return 1;
    }
}

int lookFor(struct queue *q, int dato, int *src)
{
    struct node_q *cptr = q->head;
    while (cptr)
    {
        if (cptr->message == dato)
        {
            *src = cptr->src;
            return 1;
        }
        cptr = cptr->next;
    }
    return 0;
}

void print(struct queue *q)
{
    struct node_q *cptr;
    printf("LOS VALORS DE LA COLA DE ESTE THREAD SON: ");
    cptr = q->head;
    while (cptr)
    {
        printf(" Source = %d, Mensaje = %d\n", cptr->src, cptr->message);
        cptr = cptr->next;
    }
}

void deleteQueue(struct queue *q)
{

    struct node_q *cptr;
    struct node_q *cptr2;
    cptr = q->head;
    while (cptr)
    {
        cptr2 = cptr;
        cptr = cptr->next;
        free(cptr2);
    }
    q->enqueue = q->dequeue = 0;
    q->head = q->tail = NULL;
}

void sendMessage()
{
    int mesg = rand();
    int dest = rand() % thread_count;
#pragma omp critical
    Enqueue(qq[dest], mesg, dest);
}

void TryR(int *queue_size, int rank)
{
    int *src = malloc(sizeof(int));
    int *mesg = malloc(sizeof(int));
    if (*queue_size == 0)
        return;
    else if (*queue_size == 1)
    {

        #pragma omp critical
        Dequeue(qq[rank], mesg, src, rank);
    }
    else
    {
        //printf("FLAG4\n");
        Dequeue(qq[rank], mesg, src, rank);
        //printf("FLAG5\n");
    }
    printf("El mensaje es: %d y el thread es: %d \n", *mesg, *src);
}

int Done(int *queue_size, struct queue *qqq)
{
    *queue_size = qqq->enqueue - qqq->dequeue;
    if (*queue_size == 0 && done_sending == thread_count)
        return 1;
    else
        return 0;
}

int main(int argc, char *argv[])
{
    int sent_msgs;

    int i = 0;
    for (; i < 6; i++)
    {
        qq[i] = InitQ();
    }
    //printf("YA INICIALICE LA MATRIZ DE PUNTEROS\n");
    int *queue_size;
    thread_count = strtol(argv[1], NULL, 10);
#pragma omp parallel num_threads(thread_count) private(sent_msgs, queue_size) shared(done_sending, qq)
    //rank = omp_get_thread_num();
    {
        printf("Thread %d \n", omp_get_thread_num());
        int b = 0;
        queue_size = &b;
        for (sent_msgs = 0; sent_msgs < send_max; sent_msgs++)
        {
            sendMessage();
            //printf("ENTRE AL BUCLE 1.1 \n");
            //TryR(queue_size, omp_get_thread_num());
            //printf("ENTRE AL BUCLE 1.2 \n");
        }
#pragma omp atomic
        done_sending = done_sending + 1;

        /*for(i = 0; i < 10; i++)
    {
        if(Done(queue_size, qq[omp_get_thread_num()]) == 1)
        {
            printf("FUNCA!!");
        }
        TryR(queue_size,omp_get_thread_num());
    }*/
        while (Done(queue_size, qq[omp_get_thread_num()]) == 0)
        {
            //printf("FLAG1\n");
            TryR(queue_size, omp_get_thread_num());
        }
    }

    return 0;
}