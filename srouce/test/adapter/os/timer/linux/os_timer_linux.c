#include "os.h"

#if (LIC_OS == LIC_LINUX)

#include <signal.h>
#include <sys/time.h>

/***************************************************************************
* 描述: 将触发器包装成linux格式。
***************************************************************************/
typedef struct _OsTriggerTaskSt
{
    OsTimerTaskFn fnTask;
    ALM_VOID* pPara;
}OsTriggerTaskSt;

static OsTriggerTaskSt s_stTriggerTask;

static ALM_VOID _OS_TriggerTask(ALM_INT32 iSig)
{
    if (ALM_NULL != s_stTriggerTask.fnTask)
    {
        s_stTriggerTask.fnTask(s_stTriggerTask.pPara);
    }
}
/***************************************************************************
* 描述: 以指定时间间隔进行循环触发的触发器。
***************************************************************************/
ALM_INT32 OS_StartTrigger(ALM_INT32 iCycleSecs, OsTimerTaskFn fnTrigger, ALM_VOID* pPara)
{    
    struct itimerval stItimerVal;

	s_stTriggerTask.fnTask = fnTrigger;
    s_stTriggerTask.pPara = pPara;
/*
    {
    struct sigaction stSigact; 
    stSigact.sa_handler = _OS_TriggerTask;
    stSigact.sa_flags = 0;
    sigemptyset(&stSigact.sa_mask);
    sigaction(SIGALRM, &stSigact, ALM_NULL);
    }
*/
	signal(SIGALRM, _OS_TriggerTask);
 
	
    /*quotiety of millsecond*/
    stItimerVal.it_value.tv_sec = OS_TIMER_CORE_CRYCLE_SEC;
    stItimerVal.it_value.tv_usec = 0;
    stItimerVal.it_interval = stItimerVal.it_value;

    setitimer(ITIMER_REAL, &stItimerVal, ALM_NULL);

    return 0;
}

/***************************************************************************
* 描述: 停止触发器。
***************************************************************************/
ALM_VOID OS_StopTrigger(ALM_UINT64 ulTriggerId)
{
    struct sigaction stSigact; 

    stSigact.sa_handler = ALM_NULL;
    stSigact.sa_flags   = 0;
    sigemptyset(&stSigact.sa_mask);
    sigaction(SIGALRM, &stSigact, ALM_NULL);
}

#endif /* OS */
