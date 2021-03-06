#include "def.h"

//毫秒、秒单位转换。
#define OS_LOCK_TIME_OUT_UNIT 1000 

//为了方便判断是否死锁，该处配置一个假定的超时死锁时间。
//认为超过这个假定时间，很可能已经出现死锁。单位为毫秒。
#define OS_LOCK_TIME_OUT  (1 * 60 * OS_LOCK_TIME_OUT_UNIT)

/***************************************************************************
* 描述: 需要先初始化。不允许在多线程中调用。
***************************************************************************/
ALM_INT32 OS_LockStart();

/***************************************************************************
* 描述: 关闭该模块。不允许在多线程中调用。
***************************************************************************/
ALM_INT32 OS_LockClose();

/***************************************************************************
* 描述: 创建互斥量。
***************************************************************************/
ALM_INT32 OS_LockCreate(CONST ALM_CHAR *pcName, ALM_UINT32 *pulSm);

/***************************************************************************
* 描述: 删除互斥量。
***************************************************************************/
ALM_INT32 OS_LockDelete(ALM_UINT32 ulSmId);

/***************************************************************************
* 描述: 阻塞请求互斥量。
***************************************************************************/
ALM_INT32 OS_LockP(ALM_UINT32 uiSem, ALM_UINT32 uiTimeOutInMillSec);

/***************************************************************************
* 描述: 释放互斥量。只允许拥有者线程释放锁。
***************************************************************************/
ALM_INT32 OS_LockV(ALM_UINT32 ulSmId);

/***************************************************************************
* 描述: 清理互斥量计数。
* bForce - 是否强制删除已激活Mutex.
* bAll   - 是否清除全部。
* ulSmId - 指定的Mutex编号。
***************************************************************************/
ALM_INT32 OS_LockLogCls(bool bForce, bool bAll, ALM_UINT32 ulSmId);

/***************************************************************************
* 描述: 打印互斥量使用记录。
***************************************************************************/
ALM_INT32 OS_LockLog();
