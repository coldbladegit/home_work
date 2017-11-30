#ifndef _CB_STACK_H
#define _CB_STACK_H

typedef struct _CB_STACK{
    int     size;
    int     capacity;
    int     (*push) (_CB_STACK *, void*);
    void*   (*pop) (_CB_STACK *);
    void*   (*top) (_CB_STACK *);
    bool    (*is_empty) (_CB_STACK *);
    void    **ppDatas;
}CB_STACK;

/**
* @brief    ����ջ
* @return  �ɹ�����0,���򷵻��ڴ����ʧ�ܵĴ�����
**/
int cb_new_stack(CB_STACK **ppStack);

/**
* @brief    ����ջ
* @return  û�з���ֵ
**/
void cb_delete_stack(CB_STACK *pStack);

#endif//_CB_STACK_H