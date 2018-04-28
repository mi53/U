#include "os.h"
#include "os_mutex_base.h"

#if (LIC_OS == LIC_WINDOWS)

#include <Windows.h>
#include <stdio.h>

/***************************************************************************
* 描述: 创建互斥量。
***************************************************************************/
ALM_INT32 OS_MutexCreate(CONST ALM_CHAR *pcName, OsMutexHandleSt* pstMutex)
{
    HANDLE hMutex;    

    hMutex = CreateMutex(ALM_NULL, false, (LPCTSTR)pcName);
    if (ALM_NULL == hMutex)
    {
        return RET_ERR;
    }

    pstMutex->ulCntThreadId = 0;
    pstMutex->pOsExHandle = hMutex;
    pstMutex->iThreadPVec = 0;

    return RET_OK;
}

/***************************************************************************
* 描述: 删除互斥量。
***************************************************************************/
ALM_INT32 OS_MutexDelete(OsMutexHandleSt* pstMutex)
{
    BOOL bRet;
    ALM_INT32 iRet;
    HANDLE hMutex = (HANDLE)pstMutex->pOsExHandle;

    iRet = OS_MutexP(pstMutex, 0);
    if (RET_OK != iRet)
        return RET_ERR;
    
    bRet = CloseHandle(hMutex);
    if (!bRet)
    {
        OS_MutexV(pstMutex);
        return RET_ERR;
    }

    pstMutex->pOsExHandle = ALM_NULL;
    pstMutex->ulCntThreadId = 0;
    pstMutex->iThreadPVec = 0;

    return RET_OK;
}

/***************************************************************************
* 描述: 阻塞请求互斥量。
***************************************************************************/
ALM_INT32 OS_MutexP(OsMutexHandleSt* pstMutex, ALM_UINT32 uiTimeOutInMillSec)
{
    DWORD dwRet;
    HANDLE hMutex = (HANDLE)pstMutex->pOsExHandle;
    DWORD dwThread = GetCurrentThreadId();

    if (ALM_NULL == hMutex)
    {
        return RET_ERR;
    }

    if (0 == uiTimeOutInMillSec)
    {
        uiTimeOutInMillSec =  INFINITE;
    }

    //支持线程内递归。
    if (dwThread == pstMutex->ulCntThreadId)
    {
        ++pstMutex->iThreadPVec;
        return RET_OK;
    }
    
    //得到锁。
    dwRet = WaitForSingleObject(hMutex, uiTimeOutInMillSec);
    if (WAIT_OBJECT_0 == dwRet)
    {
        //更新所有者线程。
        pstMutex->ulCntThreadId = (ALM_UINT64)dwThread;
        pstMutex->iThreadPVec = 1;

        return RET_OK;
    }

    return RET_ERR;
}


/***************************************************************************
* 描述: 阻塞互斥量。释放前序互斥量并立即P住新互斥量。
*       新互斥量请求成功后又会P住前序互斥量。
***************************************************************************/
ALM_INT32 OS_MutexPAftP(OsMutexHandleSt* pstMutex,
    ALM_UINT32 uiTimeOutInMillSec,
    OsMutexHandleSt* pstBf)
{
    DWORD dwRet;
    HANDLE hMutex = (HANDLE)pstMutex->pOsExHandle;
    HANDLE hMutexBf = (HANDLE)pstBf->pOsExHandle;
    DWORD dwThread = GetCurrentThreadId();
    DWORD dWaitTime;

    if (ALM_NULL == hMutex)
    {
        return RET_ERR;
    }

    if (0 == uiTimeOutInMillSec)
    {
        uiTimeOutInMillSec =  INFINITE;
    }

    if (dwThread != pstBf->ulCntThreadId)
        return RET_ERR;
    --pstBf->iThreadPVec;

    //支持线程内递归。
    if (dwThread == pstMutex->ulCntThreadId)
    {
        ++pstMutex->iThreadPVec;

        if (pstBf->iThreadPVec <= 0)
            ReleaseMutex(hMutexBf);
        
        return RET_OK;
    }

    dWaitTime = (DWORD)uiTimeOutInMillSec;    

    //得到锁。
    if (pstBf->iThreadPVec <= 0)
        dwRet = SignalObjectAndWait(hMutexBf, hMutex, dWaitTime, ALM_FALSE);
    else
        dwRet = WaitForSingleObject(hMutex, dWaitTime);
    if (WAIT_OBJECT_0 == dwRet)
    {
        //更新所有者线程。
        pstMutex->ulCntThreadId = (ALM_UINT64)dwThread;
        pstMutex->iThreadPVec = 1;

        return RET_OK;
    }
    else
    {
        STF_printLog("error: %d\n", GetLastError());
        return RET_ERR;
    }
}

/***************************************************************************
* 描述: 释放互斥量。只能由加锁线程释放拥有的锁。
***************************************************************************/
ALM_INT32 OS_MutexV(OsMutexHandleSt* pstMutex)
{
    BOOL bRet;
    HANDLE hMutex = (HANDLE)pstMutex->pOsExHandle;
    DWORD dwThread = GetCurrentThreadId();

    if (ALM_NULL == hMutex)
    {
        return RET_ERR;
    }
    
    if (dwThread == pstMutex->ulCntThreadId)
    {
        //释放互斥量。
        if (1 == pstMutex->iThreadPVec)
        {
            bRet = ReleaseMutex(hMutex);
            if (bRet)
            {
                pstMutex->iThreadPVec = 0;
                pstMutex->ulCntThreadId = 0;
                return RET_OK;
            }
            else
            {
                STF_printLog("error: %d\n", GetLastError());
                return RET_ERR;
            }
        }
        else if (pstMutex->iThreadPVec > 1)
        {
            --pstMutex->iThreadPVec;
            return RET_OK;
        }
        else
        {
            pstMutex->iThreadPVec = 0;
            pstMutex->ulCntThreadId = 0;
            return RET_OK;
        }
    }
    else
    {
        ReleaseMutex(hMutex);
        //.............
        return RET_OK;
    }
}


#endif /* OS */
