/**************************************************************************
	filename : 	src\base\os_adapter\thread\thread.h
	file path:	src\base\os_adapter\thread
	file base:	thread
	file ext :	h

	purpose  :	适配不同操作系统的线程处理函数。
***************************************************************************/
#include "os_thread.h"
#include "util.h"
#include <stdio.h>


/***************************************************************************
* 描述: 启动一个新的线程。
***************************************************************************/
HThread thread_start(thread_fn threadfn, ALM_VOID* pData)
{
    HThread hThread;

#if (LIC_WINDOWS == LIC_OS)
    hThread = CreateThread(0,0,(LPTHREAD_START_ROUTINE)threadfn,pData,0,0);
#elif (LIC_LINUX == LIC_OS)
    ALM_INT32 err;
    err = pthread_create(&hThread,0,threadfn,pData);
    if (err != 0)
    {
        OS_LOG("ERROR: create thread failure.\n");
    }
#endif
    
    return hThread;
}

/***************************************************************************
* 描述: 等待指定线程运行结束。
***************************************************************************/
ALM_VOID thread_wait(HThread* pThread)
{
    if (ALM_NULL == pThread)
    {
        OS_LOG_ERR("Thread handle is null.");
        return;
    }
    
#if (LIC_WINDOWS == LIC_OS)
    WaitForSingleObject(*pThread, INFINITE);
#elif (LIC_LINUX == LIC_OS)
    ALM_VOID* retVal = 0;
    pthread_join(*pThread, &retVal);
#endif
}


/************************************************************************/
/* 获取当前线程编号。
/************************************************************************/
HThread Os_ThreadGetId()
{
    HThread hThread;

#if (LIC_WINDOWS == LIC_OS)
    hThread = GetCurrentThread();
#elif (LIC_LINUX == LIC_OS)
    hThread = pthread_self();
#endif

    return hThread;
}
