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
* @brief    创建栈
* @return  成功返回0,否则返回内存分配失败的错误码
**/
int cb_new_stack(CB_STACK **ppStack);

/**
* @brief    销毁栈
* @return  没有返回值
**/
void cb_delete_stack(CB_STACK *pStack);

#endif//_CB_STACK_H