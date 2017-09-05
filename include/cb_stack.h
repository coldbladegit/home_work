#ifndef _CB_STACK_H
#define _CB_STACK_H

/**
* @brief    ����ջ
* @return  �ɹ�����0,���򷵻��ڴ����ʧ�ܵĴ�����
**/
int cb_create_stack(void **ppStack);

/**
* @brief    ����ջ
* @return  û�з���ֵ
**/
void cb_destroy_stack(void *pStack);

/**
* @brief    ѹջ,���ջ��ǰ�ռ���,���ȥ��̬���ӿռ�
* @return  �ɹ�����0,���򷵻��ڴ����ʧ�ܵĴ�����
**/
int cb_stack_push(void *pStack, void *pData);

/**
* @brief    ��ջ
* @return  �ɹ�����ջ��Ԫ��,ʧ�ܷ���NULL
**/
void* cb_stack_pop(void *pStack);

/**
* @brief    �ж�ջ�Ƿ�Ϊ��
* @return  Ϊ�շ���true,���򷵻�false
**/
bool cb_is_stack_empty(void *pStack);

#endif//_CB_STACK_H