#ifndef _RECYCLE_QUEUE_H
#define _RECYCLE_QUEUE_H

typedef struct _RECYCLE_QUEUE {
    int size;
    int addIndex;
    int rmIndex;
    int capacity;
    int (*Add)(_RECYCLE_QUEUE *, char *);
    char *(*Remove)(_RECYCLE_QUEUE *);
    bool (*IsEmpty)(_RECYCLE_QUEUE *);
    bool (*IsFull)(_RECYCLE_QUEUE *);
    char **strArry;
}RECYCLE_QUEUE;

int CreateRecyleQueue(RECYCLE_QUEUE **ppRecyQue, int capacity);

void DestroyRecyleQueue(RECYCLE_QUEUE *pRecyQue);

#endif//_RECYCLE_QUEUE_H