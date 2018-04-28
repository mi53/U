#include "os.h"

#if (LIC_OS == LIC_WINDOWS)

#include <windows.h>


#pragma comment(lib,"Winmm.lib")


/***************************************************************************
* 描述: 将触发器包装成windows格式。
***************************************************************************/
typedef struct _OsTriggerTaskSt
{
    OsTimerTaskFn fnTask;
    ALM_VOID* pPara;
}OsTriggerTaskSt;

static ALM_VOID WINAPI _OS_TriggerTask(UINT uTimerID,
                                     UINT uMsg,
                                     DWORD_PTR dwUser,
                                     DWORD_PTR dw1,
                                     DWORD_PTR dw2)
{
    OsTriggerTaskSt* pstPara = (OsTriggerTaskSt*)dwUser;
    
    if (ALM_NULL != pstPara && ALM_NULL != pstPara->fnTask)
    {
        pstPara->fnTask(pstPara->pPara);
    }

    (ALM_VOID)dw1;
    (ALM_VOID)dw2;
    (ALM_VOID)uMsg;
    (ALM_VOID)uTimerID;
}

/***************************************************************************
* 描述: 以指定时间间隔进行循环触发的触发器。
***************************************************************************/
ALM_INT32 OS_StartTrigger(ALM_INT32 iCycleSecs, OsTimerTaskFn fnTrigger, ALM_VOID* pPara)
{
    ALM_UINT32 ulTimerId;
    static OsTriggerTaskSt stTask;

    stTask.fnTask = fnTrigger;
    stTask.pPara = pPara;
    ulTimerId = timeSetEvent(
        1000 * iCycleSecs,
        1000,
        _OS_TriggerTask,
        (DWORD_PTR)&stTask,
        TIME_PERIODIC);

    return ulTimerId;
}

/***************************************************************************
* 描述: 停止触发器。
***************************************************************************/
ALM_VOID OS_StopTrigger(ALM_UINT64 ulTriggerId)
{
    timeKillEvent((UINT)ulTriggerId);
}

#endif /* OS */
