#include "os.h"
#include "os_mutex.h"
#include "os_mutex_base.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>

typedef struct _OsLockInfoSt
{
    OsMutexHandleSt stLockHandle;

    ALM_CHAR* pcName;
    ALM_INT64 lPCount;
    ALM_INT64 lPOkCount;
    ALM_INT64 lVCount;
    ALM_INT64 lVOkCount;
    bool bIsAct;
    bool bLocked;

    bool bNotFreeNode; //存储锁的结点是否有效，空闲:无效。
    //OsMutexHandleSt stSafeLock;
}OsLockInfoSt;

static OsLockInfoSt s_stOsLockSafe;
static OsLockInfoSt* s_pstLockList;
static ALM_INT32 s_iLockCapT;

/***************************************************************************
* 描述: 记录器相关操作函数。
***************************************************************************/
static ALM_INT32 _OS_LockLogGetFree(); //返回索引，失败时返回-1。
static ALM_INT32 _OS_LockLogGetByName(CONST ALM_CHAR* pcLockName); //返回索引，失败时返回-1。
static OsLockInfoSt* _OS_LockLogGetById(ALM_UINT64 ulSmId); //返回有效对象。
static ALM_INT32 _OS_MutexP(OsLockInfoSt* pstLock, ALM_UINT32 ulTimeOutInMillSec);
static ALM_INT32 _OS_MutexPAft(OsLockInfoSt* pstLock, ALM_UINT32 ulTimeOutInMillSec, OsLockInfoSt* pstBf);

#define LOCK_SAFE_P() \
    iRet = _OS_MutexP(&s_stOsLockSafe, 0);\
    OS_CALL_FN_RET("_OS_MutexP", iRet)

#define LOCK_SAFE_V() \
    iRet = OS_MutexV(&s_stOsLockSafe.stLockHandle);\
    OS_CALL_FN_RET("OS_MutexV", iRet)
/***************************************************************************
* 描述: 需要先初始化。不允许在多线程中调用。
***************************************************************************/
ALM_INT32 OS_LockStart()
{
    ALM_INT32 iRet;

    iRet = OS_MutexCreate("Lock_Log_Saft_Mutex", &s_stOsLockSafe.stLockHandle);
    OS_CALL_FN_RET("OS_MutexCreate", iRet);

    s_stOsLockSafe.pcName = "s_stOsLockSafe";

    return iRet;
}

/***************************************************************************
* 描述: 关闭该模块。不允许在多线程中调用。
***************************************************************************/
ALM_INT32 OS_LockClose()
{
    ALM_INT32 iRet;
    ALM_INT32 itr;

    //记录器保护。
    LOCK_SAFE_P();

    //清除记录。
    for (itr = 0; itr < s_iLockCapT; ++itr)
    {
        if (s_pstLockList[itr].bNotFreeNode)
        {
            if (s_pstLockList[itr].bIsAct)
            {
                iRet = OS_MutexDelete(&s_pstLockList[itr].stLockHandle);
                OS_CALL_FN_LAB("OS_MutexDelete", iRet, LAB_END);
            }
            Util_Free(s_pstLockList[itr].pcName);
        }
        memset(s_pstLockList + itr, 0, sizeof(OsLockInfoSt));
    }
    Util_Free(s_pstLockList);
    s_pstLockList = ALM_NULL;

    //删除记录器保护互斥量。
    iRet = OS_MutexDelete(&s_stOsLockSafe.stLockHandle);
    OS_CALL_FN_LAB("OS_MutexDelete", iRet, LAB_END);

    return iRet;

LAB_END:
    LOCK_SAFE_V();

    return iRet;

}

/***************************************************************************
* 描述: 创建互斥量。
***************************************************************************/
ALM_INT32 OS_LockCreate(CONST ALM_CHAR *pcName, ALM_UINT32 *pulSm)
{
    ALM_INT32 iRet;
    OsLockInfoSt* pstNewLock = ALM_NULL;
    ALM_INT32 iLockIdx;

    LOCK_SAFE_P();

    //判断是否存在。
    iLockIdx = _OS_LockLogGetByName(pcName);

    //创建记录。
    if (iLockIdx < 0)
    {
        iLockIdx = _OS_LockLogGetFree();
        OS_CHK(iLockIdx >= 0);
        
        pstNewLock = s_pstLockList + iLockIdx;
        memset(pstNewLock, 0, sizeof(OsLockInfoSt));

        pstNewLock->pcName = Util_StrCpy(pcName);
        pstNewLock->bIsAct = false;
        pstNewLock->bNotFreeNode = true;
    }
    else
        pstNewLock = s_pstLockList + iLockIdx;

    //创建新的锁。
    if (!pstNewLock->bIsAct)
    {
        iRet = OS_MutexCreate(pcName, &pstNewLock->stLockHandle);
        OS_CALL_FN_LAB("OS_MutexCreate", iRet, LAB_END);

        pstNewLock->bIsAct = true;
    }
    else
        OS_LOG_INFO("Create a actived mutex: %s.", pstNewLock->pcName);

    *pulSm = iLockIdx;

LAB_END:
    LOCK_SAFE_V();

    return iRet;
}

/***************************************************************************
* 描述: 删除锁。不清理记录。
***************************************************************************/
ALM_INT32 OS_LockDelete(ALM_UINT32 ulSemId)
{
    ALM_INT32 iRet;
    OsLockInfoSt* pstLock = ALM_NULL;
    bool bNeedFree;
    OsLockInfoSt stLockTmp;

    LOCK_SAFE_P();

    pstLock = _OS_LockLogGetById(ulSemId);
    if (ALM_NULL == pstLock)
    {
        iRet = RET_ERR;
        goto LAB_END;
    }    
    
    bNeedFree = pstLock->bIsAct;
    pstLock->bIsAct = false;
    pstLock->bNotFreeNode = false;

    free(pstLock->pcName);
    pstLock->pcName = ALM_NULL;
    memcpy(&stLockTmp, pstLock, sizeof(stLockTmp));
    pstLock = &stLockTmp;

    LOCK_SAFE_V();
    
    //删除。
    if (bNeedFree)
    {
        iRet = OS_MutexDelete(&pstLock->stLockHandle);
        OS_CALL_FN_LAB("OS_MutexDelete", iRet, LAB_END);
    }

LAB_END:
    return iRet;
}

/***************************************************************************
* 描述: 请求互斥量。
***************************************************************************/
ALM_INT32 OS_LockP(ALM_UINT32 ulSemId, ALM_UINT32 ulTimeOutInMillSec)
{
    OsLockInfoSt* pstLock = ALM_NULL;
    ALM_INT32 iRet;
    OsLockInfoSt stLockTmp;
    
    //更新锁记录。
    LOCK_SAFE_P();

    pstLock = _OS_LockLogGetById(ulSemId);
    if (ALM_NULL == pstLock)
    {
        iRet = RET_ERR;
        goto LAB_END;
    }
    ++pstLock->lPCount;

    memcpy(&stLockTmp, pstLock, sizeof(stLockTmp));
    pstLock = &stLockTmp;

    LOCK_SAFE_V();

    //加锁。 
    iRet = OS_MutexP(&pstLock->stLockHandle, ulTimeOutInMillSec);
    OS_CALL_FN_RET("_OS_MutexP", iRet);

    //重新获得管理锁，数据可能被更新。
    LOCK_SAFE_P();
    pstLock = _OS_LockLogGetById(ulSemId);
    if (ALM_NULL == pstLock)
    {
        //锁记录被删除。
        iRet = RET_ERR;
        goto LAB_END;
    }
    ++pstLock->lPOkCount;

LAB_END:
    LOCK_SAFE_V();

    if (RET_OK != iRet)
        OS_LOG_INFO("lock %s fial.", pstLock->pcName);
    

    return iRet;
}

/***************************************************************************
* 描述: 释放互斥量。只允许拥有者线程释放锁。
***************************************************************************/
ALM_INT32 OS_LockV(ALM_UINT32 ulSmId)
{
    ALM_INT32 iRet;
    OsLockInfoSt* pstLock = s_pstLockList;

    //锁保护。
    LOCK_SAFE_P();

    pstLock = _OS_LockLogGetById(ulSmId);
    if (ALM_NULL == pstLock)
    {
        iRet = RET_ERR;
        goto LAB_END;
    }
   
    ++pstLock->lVCount;

    //解锁。
    iRet = OS_MutexV(&pstLock->stLockHandle);    
    OS_CALL_FN_LAB("OS_MutexV", iRet, LAB_END);
    ++pstLock->lVOkCount;

LAB_END:

    if (RET_OK != iRet)
        OS_LOG_INFO("V lock %s fial.", pstLock->pcName);
    
    LOCK_SAFE_V();
    return iRet;
}

/***************************************************************************
* 描述: 清理互斥量计数。
* bForce - 是否强制删除已激活Mutex.
* bAll   - 是否清除全部。
* ulSmId - 指定的Mutex编号。
***************************************************************************/
ALM_INT32 OS_LockLogCls(bool bForce, bool bAll, ALM_UINT32 ulSmId)
{
    ALM_INT32 iRet;
    ALM_INT32 idx;
    OsLockInfoSt* pstLock = ALM_NULL;

    LOCK_SAFE_P();

    //清除指定。
    if (!bAll)
    {
        pstLock = _OS_LockLogGetById(ulSmId);
        if (ALM_NULL != pstLock
            && pstLock->bNotFreeNode
            && (bForce || !pstLock->bIsAct))
        {
            pstLock->lPCount = 0;
            pstLock->lPOkCount = 0;
            pstLock->lVCount = 0;
            pstLock->lVOkCount = 0;
        }
    }
    else //清除全部。
    {
        for (idx = 0; idx < s_iLockCapT; ++idx)
        {
            pstLock = s_pstLockList + idx;

            if (pstLock->bNotFreeNode
                && (bForce || !pstLock->bIsAct))
            {
                pstLock->lPCount = 0;
                pstLock->lPOkCount = 0;
                pstLock->lVCount = 0;
                pstLock->lVOkCount = 0;
            }
        }
    }

    LOCK_SAFE_V();

    return RET_OK;
}

/***************************************************************************
* 描述: 打印互斥量使用记录。
***************************************************************************/
ALM_INT32 OS_LockLog()
{
    ALM_INT32 iRet;
    ALM_INT32 idx;
    OsLockInfoSt* pstLock = ALM_NULL;

    LOCK_SAFE_P();

    OS_LOG("%-20s %-2s %-5s %-6s %-6s %-6s %-6s\n",
        "Name",
        "Id",
        "Valid",
        "P Tol",
        "V Tol",
        "P OK",
        "V OK");
    OS_LOG("%-20s %-2s %-5s %-6s %-6s %-6s %-6s\n",
        "====================",
        "==",
        "=====",
        "======",
        "======",
        "======",
        "======");

    for (idx = 0; idx < s_iLockCapT; ++idx)
    {
        pstLock = s_pstLockList + idx;

        OS_LOG("%-20s %-2u %-5s %-6d %-6d %-6d %-6d\n",
            pstLock->pcName,
            idx,
            pstLock->bIsAct ? "yes" : "no",
            pstLock->lPCount,
            pstLock->lVCount,
            pstLock->lPOkCount,
            pstLock->lVOkCount);       
    }

    LOCK_SAFE_V();

    return RET_OK;
}

//////////////////////////////////////////////////////////////////////////////

static ALM_INT32 _OS_LockLogGetFree()
{
    ALM_INT32 idx;
    OsLockInfoSt* pstNew = ALM_NULL;

    for (idx = 0; idx < s_iLockCapT; ++idx)
    {
        if (!s_pstLockList[idx].bNotFreeNode)
        {
            return idx;
        }
    }

    //扩容。
    pstNew = (OsLockInfoSt*)malloc(sizeof(OsLockInfoSt) * (2 * s_iLockCapT + 1));
    memset(pstNew, 0, sizeof(OsLockInfoSt) * (2 * s_iLockCapT + 1));
    if (s_iLockCapT > 0)
        memcpy(pstNew, s_pstLockList, sizeof(OsLockInfoSt) * s_iLockCapT);
    Util_Free(s_pstLockList);
    s_pstLockList = pstNew;
    s_iLockCapT = 2 * s_iLockCapT + 1;

    return idx;
}

static ALM_INT32 _OS_LockLogGetByName(CONST ALM_CHAR* pcLockName)
{
    ALM_INT32 idx;
    ALM_INT32 iCmp;

    for (idx = 0; idx < s_iLockCapT; ++idx)
    {
        if (s_pstLockList[idx].bNotFreeNode)
        {
            iCmp = strcmp(pcLockName, s_pstLockList[idx].pcName);
            if (0 == iCmp)
            {
                return idx;
            }
        }
    }

    return -1;
}

static OsLockInfoSt* _OS_LockLogGetById(ALM_UINT64 ulSmId)
{
    if (ulSmId < (ALM_UINT32)s_iLockCapT
        && s_pstLockList[ulSmId].bNotFreeNode)
    {
        return s_pstLockList + ulSmId;
    }
    
    return ALM_NULL;
}

static ALM_INT32 _OS_MutexP(OsLockInfoSt* pstLock, ALM_UINT32 ulTimeOutInMillSec)
{
    ALM_UINT32 ulLeft = 0;
    ALM_INT32 iRet = RET_OK;

    if (0 == ulTimeOutInMillSec || ulTimeOutInMillSec > OS_LOCK_TIME_OUT)
    {
        iRet = OS_MutexP(&(pstLock->stLockHandle), OS_LOCK_TIME_OUT);
        if (RET_OK != iRet)
        {
            OS_LOG_INFO("lock %s - %d timeout %d secs.",
                pstLock->pcName,
                pstLock->stLockHandle.ulCntThreadId,
                OS_LOCK_TIME_OUT / OS_LOCK_TIME_OUT_UNIT);

            if (ulTimeOutInMillSec > OS_LOCK_TIME_OUT)
                ulLeft = ulTimeOutInMillSec - OS_LOCK_TIME_OUT;
            
            iRet = OS_MutexP(&(pstLock->stLockHandle), ulLeft);
            OS_CALL_FN_RET("_OS_MutexP", iRet);
        }
    }
    else
    {
        iRet = OS_MutexP(&(pstLock->stLockHandle), ulTimeOutInMillSec);
        OS_CALL_FN_RET("_OS_MutexP", iRet);
    }

    return RET_OK;
}

static ALM_INT32 _OS_MutexPAft(OsLockInfoSt* pstLock,
                         ALM_UINT32 ulTimeOutInMillSec,
                         OsLockInfoSt* pstBf)
{
    ALM_UINT32 ulLeft = 0;
    ALM_INT32 iRet = RET_OK;

    if (0 == ulTimeOutInMillSec || ulTimeOutInMillSec > OS_LOCK_TIME_OUT)
    {
        iRet = OS_MutexPAftP(&(pstLock->stLockHandle),
            OS_LOCK_TIME_OUT,
            &(pstBf->stLockHandle));
        if (RET_OK != iRet)
        {
            OS_LOG_INFO("lock %s - %d timeout %d secs.",
                pstLock->pcName,
                pstLock->stLockHandle.ulCntThreadId,
                OS_LOCK_TIME_OUT / OS_LOCK_TIME_OUT_UNIT);

            if (ulTimeOutInMillSec > OS_LOCK_TIME_OUT)
                ulLeft = ulTimeOutInMillSec - OS_LOCK_TIME_OUT;

            iRet = OS_MutexP(&(pstLock->stLockHandle), ulLeft);
            OS_CALL_FN_RET("_OS_MutexP", iRet);
        }
    }
    else
    {
        iRet = OS_MutexPAftP(&(pstLock->stLockHandle),
            ulTimeOutInMillSec,
            &(pstBf->stLockHandle));
        OS_CALL_FN_RET("_OS_MutexP", iRet);
    }

    return RET_OK;
}
