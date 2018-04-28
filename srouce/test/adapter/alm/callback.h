#ifndef _call_back_h_
#define _call_back_h_

#include "lic_inc.h"

//设置ESN。
ALM_INT32 Adapter_SetEsn(const char* pcEsn);

//得到ESN。
ALM_INT32 Adapter_GetEsn(OUT ALM_ENV_ESN_STRU* pstEsn);

//实现持久性写。
ALM_INT32 Adapter_PsIOWrite(CONST ALM_VOID* pBuf,
    IN ALM_UINT32 uiLen, 
    IN ALM_UINT32 uiOffSet);

//实现持久化读。
ALM_INT32 Adapter_PsIORead(OUT ALM_VOID *pBuf,
    INOUT ALM_UINT32 *puiLen,
    IN ALM_UINT32 uiOffSet);

//清楚持久化记录。
ALM_INT32 Adapter_PsCls();

/* 打印日志。 */
ALM_INT32 Adapter_Log(ALM_ENUM(ALM_LOG_LEVEL_ENUM) eLevel,
    CONST ALM_CHAR* pcLogStr);

/* 状态变更通知 */
ALM_INT32 Adapter_NotifyChange(CONST ALM_NOTIFY_CHANGES_STRU* pstChanges);

/*告警通知*/
ALM_INT32 Adapter_Alarm(CONST ALM_ALARM_PRD_CHANGE_STRU* pstArray,
    ALM_UINT32 uiNum);


/* 将原因附加操作转换为原因字符串 */
static CONST ALM_CHAR* ALMDbgRsnAndOptToRsnStr(
    ALM_ENUM(ALM_STATE_OPERATE_ENUM) eOperate,
    ALM_UINT64 ulRsn,
    OUT ALM_CHAR* pcRsn,
    ALM_UINT32 uiRsnSize);

#endif
