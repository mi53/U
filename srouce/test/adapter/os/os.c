#include "os.h"


/***************************************************************************
* 描述: OS启动。lock -> timer -> simula date
***************************************************************************/
ALM_INT32 OS_Start()
{
    OS_LockStart();
    OS_TimerStart();
    OS_DateSimulaStart();

    return RET_OK;
}

/***************************************************************************
* 描述: OS关闭。simula date -> timer -> lock
***************************************************************************/
ALM_INT32 OS_Close()
{
    OS_MemAlmLog();

    //OS_LockLog();
    OS_DateSimulaClose();
    OS_TimerClose();
    OS_LockClose();

    return RET_OK;
}
