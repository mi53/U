#include "os.h"
#include <string.h>
#include <util.h>


/* timer structure，以秒计时。 */
typedef struct _OsTimerSt
{
    /* 定时器编号 */
    ALM_UINT32 ulTimerId;

    /* 超时周期 */
    ALM_UINT32 ulTimerTotal;

    /* 定时器超时剩余时间  */
    ALM_INT32 lTimerLeft;

    /* timer mode, once or loop */
    ALM_UINT32 ulTimerMode;

    /* timer callback */
    OsTimerTaskFn pfTimerFunc;
}OsTimerSt;


//定时器统一使用的触发器编号。
static ALM_UINT32 s_ulTimerTriggerId = 0;

//定时器保护锁。
static ALM_UINT32 s_ulTimerLock = 0;


//定时器存储器。
static OsTimerSt s_astTimers[OS_TIMER_MAX_NUM] = {0};

//当前定时器数目。
static ALM_INT32 s_iTimerCnt = 0;

//是否挂起全部定时器。
static bool s_bTimerPause = false;

//定时器保护锁使用的名字。
#define OS_TIMER_LOCK_NAME "OS_TIMER_LOCK_NAME"

/***************************************************************************
* 描述: 触发全部已超时的定时器。
***************************************************************************/

//定时器任务的具体事务。
static ALM_VOID _OS_TimerTask(ALM_VOID* pPara, bool bAuto)
{
    ALM_INT32 iRet = RET_OK;
    ALM_INT32 iIter = 0;

    if (bAuto && s_bTimerPause)
        return;

    if (bAuto)
    {
        iRet = OS_LockP(s_ulTimerLock, 0);
        if (RET_ERR == iRet)
            return;
    }

    //寻找超时定时器，并执行定时器回调。
    for (iIter = 0; iIter < s_iTimerCnt; iIter++)
    {
        s_astTimers[iIter].lTimerLeft -= (ALM_INT64)OS_TIMER_CORE_CRYCLE_SEC;
        if (s_astTimers[iIter].lTimerLeft <= 0)
        {
            s_astTimers[iIter].lTimerLeft = s_astTimers[iIter].ulTimerTotal;
            s_astTimers[iIter].pfTimerFunc(0);
        }
    }
    iRet = OS_LockV(s_ulTimerLock);
    
    if (bAuto)
    {
        (ALM_VOID)pPara;
    }
}

//定时器自动任务。
static ALM_VOID _OS_TimerTaskByAuto(ALM_VOID* pPara)
{
    _OS_TimerTask(pPara, true);
}

/***************************************************************************
* 描述: 初始化定时器模块。
***************************************************************************/
ALM_INT32 OS_TimerStart()
{
    ALM_INT32 iRet;

    //初始化定时器保护锁。
    iRet = OS_LockCreate(OS_TIMER_LOCK_NAME, &s_ulTimerLock);
    if (RET_ERR == iRet)
    {
        return RET_ERR;
    }

    //初始化触发器，需要用锁保护。
    iRet = OS_LockP(s_ulTimerLock, 0);
    if (RET_ERR == iRet)
        return RET_ERR;

    s_ulTimerTriggerId = OS_StartTrigger(
        OS_TIMER_CORE_CRYCLE_SEC,
        _OS_TimerTaskByAuto,
        ALM_NULL);
    if (0 == s_ulTimerTriggerId)
    {
        iRet = OS_LockV(s_ulTimerLock);
        return RET_ERR;
    }

    iRet = OS_LockV(s_ulTimerLock);
    return RET_ERR;
}

/***************************************************************************
* 描述: 关闭定时器模块。
***************************************************************************/
ALM_INT32 OS_TimerClose()
{
    ALM_INT32 iRet;

    //初始化触发器，需要用锁保护。
    iRet = OS_LockP(s_ulTimerLock, 0);
    if (RET_ERR == iRet)
        return RET_ERR;

    OS_StopTrigger(s_ulTimerTriggerId);
    iRet = OS_LockV(s_ulTimerLock);

    OS_LockDelete(s_ulTimerLock);
    s_ulTimerLock = 0;
    s_ulTimerTriggerId = 0;

    return RET_ERR;
}

/***************************************************************************
* 描述: 启动一个周期任务相对定时器。时间间隔单位为毫秒。
***************************************************************************/
ALM_INT32 OS_TimerNew(ALM_UINT32 uiTimerId,
    ALM_UINT32 uiCycleMSecs,
    OsTimerTaskFn pfnTmCallBack,
    ALM_VOID* pData,
    ALM_UINT32* puiMaxLockMSecs)
{
    OsTimerSt *pstTimer;
    ALM_INT32 iRet;
    ALM_INT32 index;
    
    (void)pData;

    if (ALM_NULL != puiMaxLockMSecs)
    {
        *puiMaxLockMSecs = 1000;
    }

    iRet = OS_LockP(s_ulTimerLock, 0);
    if (RET_ERR == iRet)
        return RET_ERR;

    if (s_iTimerCnt >= OS_TIMER_MAX_NUM)
    {
        iRet = OS_LockV(s_ulTimerLock);
        return RET_ERR;
    }

    //判断是否已经存在。
    for (index = 0; index < s_iTimerCnt; ++index)
    {
        if (uiTimerId == s_astTimers[index].ulTimerId)
        {
            iRet = OS_LockV(s_ulTimerLock);
            return RET_ERR;
        }
    }

    //在末尾添加一个定时器
    pstTimer = &(s_astTimers[s_iTimerCnt]);
    memset(pstTimer, 0, sizeof(OsTimerSt));

    pstTimer->ulTimerId = uiTimerId;
    pstTimer->pfTimerFunc = pfnTmCallBack;
    pstTimer->ulTimerMode = 0;
    pstTimer->ulTimerTotal = uiCycleMSecs / 1000;//转换成秒
    pstTimer->lTimerLeft = pstTimer->ulTimerTotal;

    s_iTimerCnt++;

    iRet = OS_LockV(s_ulTimerLock);

    return RET_OK;
}


/***************************************************************************
* 描述: 停止定时器。
***************************************************************************/
ALM_INT32 OS_TimerDel(ALM_UINT32 ulTmHandler)
{
    ALM_UINT64 ulTimerId = ulTmHandler;//ID与HANDLE相同。
    ALM_INT32 index;
    ALM_INT32 iRet;
    ALM_INT32 iAfTimerCnt = 0;
    OsTimerSt* pstCpyTmp = ALM_NULL;

    iRet = OS_LockP(s_ulTimerLock, 0);
    if (RET_ERR == iRet)
        return RET_ERR;

    if (s_iTimerCnt >= OS_TIMER_MAX_NUM)
    {
        iRet = OS_LockV(s_ulTimerLock);
        return RET_ERR;
    }
    else if (s_iTimerCnt <= 0)
    {
        s_iTimerCnt = 0;
        iRet = OS_LockV(s_ulTimerLock);
        return RET_ERR;
    }


    //寻找并删除定时器。
    for (index = 0; index < s_iTimerCnt; ++index)
    {
        if (ulTimerId == s_astTimers[index].ulTimerId)
        {
            //后续定时器向前挪威，以便支持定时器在末尾添加。
            iAfTimerCnt = s_iTimerCnt - index - 1;
            if (iAfTimerCnt > 0)
            {
                pstCpyTmp = (OsTimerSt*)OS_MemAlloc_M(iAfTimerCnt * sizeof(OsTimerSt));
                memcpy(pstCpyTmp,
                    s_astTimers + index + 1,
                    iAfTimerCnt * sizeof(OsTimerSt));
                memcpy(s_astTimers + index,
                    pstCpyTmp,
                    iAfTimerCnt * sizeof(OsTimerSt));
                Util_Free(pstCpyTmp);
            }

            --s_iTimerCnt;

            iRet = OS_LockV(s_ulTimerLock);
            return RET_OK;
        }
    }

    iRet = OS_LockV(s_ulTimerLock);
    return RET_ERR;
}

/***************************************************************************
* 描述: 增加(iSecs/ALM_TIMER_SEG_SECOND)*ALM_TIMER_SEG_SECOND 的时间量，
*       并同时模拟触发全部定时器。
***************************************************************************/
ALM_VOID OS_TimerRunSecs(ALM_INT32 iSecs)
{
    ALM_INT32 iRunCount = iSecs / OS_TIMER_CORE_CRYCLE_SEC;

    OS_LockP(s_ulTimerLock, 0);

    while (iRunCount-- > 0)
        _OS_TimerTask(ALM_NULL, false);

    OS_LockV(s_ulTimerLock);
}

//阻塞全部定时器。但允许使用函数OS_TimerRunSecs调用。
ALM_VOID OS_TimerBlock()
{
    s_bTimerPause = true;
}

//恢复全部定时器。
ALM_VOID OS_TimerRecover()
{
    s_bTimerPause = false;
}
