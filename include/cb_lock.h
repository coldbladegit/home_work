#ifndef _CB_LOCK_H_
#define _CB_LOCK_H_

typedef void  * LOCK_HANDLE;

/**
* @brief   ��������ʼ��һ����
* @return  ����һ����
**/
LOCK_HANDLE cb_createLock();

/**
* @brief   ����
* @return  û�з���ֵ
**/
void cb_lock(LOCK_HANDLE lock);

/**
* @brief   ����
* @return  û�з���ֵ
**/
void cb_unlock(LOCK_HANDLE lock);

/**
* @brief   ������
* @return  û�з���ֵ
**/
void cb_destroyLock(LOCK_HANDLE lock);

#endif