#ifndef _CB_STACK_H
#define _CB_STACK_H

/**
* @brief    创建栈
* @return  成功返回0,否则返回内存分配失败的错误码
**/
int cb_create_stack(void **ppStack);

/**
* @brief    销毁栈
* @return  没有返回值
**/
void cb_destroy_stack(void *pStack);

/**
* @brief    压栈,如果栈当前空间满,则会去动态增加空间
* @return  成功返回0,否则返回内存分配失败的错误码
**/
int cb_stack_push(void *pStack, void *pData);

/**
* @brief    出栈
* @return  成功返回栈顶元素,失败返回NULL
**/
void* cb_stack_pop(void *pStack);

/**
* @brief    判断栈是否为空
* @return  为空返回true,否则返回false
**/
bool cb_is_stack_empty(void *pStack);

#endif//_CB_STACK_H