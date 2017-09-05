#ifndef _CB_LOCK_H_
#define _CB_LOCK_H_

typedef void  * LOCK_HANDLE;

/**
* @brief   创建并初始化一个锁
* @return  返回一个锁
**/
LOCK_HANDLE cb_createLock();

/**
* @brief   入锁
* @return  没有返回值
**/
void cb_lock(LOCK_HANDLE lock);

/**
* @brief   出锁
* @return  没有返回值
**/
void cb_unlock(LOCK_HANDLE lock);

/**
* @brief   销毁锁
* @return  没有返回值
**/
void cb_destroyLock(LOCK_HANDLE lock);

#endif