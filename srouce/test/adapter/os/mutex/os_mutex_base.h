#ifndef _os_mutex_base_h_
#define _os_mutex_base_h_

#include "def.h"

typedef struct _OsMutexHandleSt
{
    ALM_VOID* pOsExHandle;
    ALM_UINT64 ulCntThreadId;
    ALM_INT32 iThreadPVec;//线程内P-V的次数。
}OsMutexHandleSt;

/***************************************************************************
* 描述: 创建互斥量。
***************************************************************************/
ALM_INT32 OS_MutexCreate(CONST ALM_CHAR *pcName, OsMutexHandleSt* pstMutex);

/***************************************************************************
* 描述: 删除互斥量。
***************************************************************************/
ALM_INT32 OS_MutexDelete(OsMutexHandleSt* pstMutex);

/***************************************************************************
* 描述: 阻塞请求互斥量。
***************************************************************************/
ALM_INT32 OS_MutexP(OsMutexHandleSt* pstMutex, ALM_UINT32 uiTimeOutInMillSec);

/***************************************************************************
* 描述: 释放互斥量。
*****************************************************.**********************/
ALM_INT32 OS_MutexV(OsMutexHandleSt* pstMutex);

/***************************************************************************
* 描述: 阻塞互斥量。释放前序互斥量并立即P住新互斥量。
*       新互斥量请求成功后又会P住前序互斥量。
***************************************************************************/
ALM_INT32 OS_MutexPAftP(OsMutexHandleSt* pstMutex,
                  ALM_UINT32 uiTimeOutInMillSec,
                  OsMutexHandleSt* pstBf);


#endif // _os_mutex_base_h_
