/*******************************************************************************
History        :
<version>  <date>    <Author>     <Description>
*******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#include "callback.h"

static ALM_ENV_ESN_STRU s_stEsn = {"esn0"};
ALM_ALARM_PRD_CHANGE_STRU g_stAlarmChangeTem = {0};
ALM_ALARM_LK_STRU g_stRecoveryLkAlarmTem;
ALM_ALARM_LK_STRU g_stFaultLkAlarmTem;
ALM_ALARM_BASE_STRU g_stEmAlarmTem;
ALM_ALARM_BASE_STRU g_stStickAlarmTem;
ALM_ALARM_FEATURE_STRU g_stFeatureAlarmTem;
ALM_ALARM_BASE_FEATURE_STRU g_stBaseFeatureRec;
ALM_ALARM_BASE_FEATURE_STRU g_stBaseFeatureFault;

//设置ESN。
ALM_INT32 Adapter_SetEsn(const char* pcEsn)
{
    strncpy(s_stEsn.acEsn, pcEsn, sizeof(s_stEsn.acEsn));
    s_stEsn.acEsn[sizeof(s_stEsn.acEsn) - 1] = '\0';

    return ALM_OK;
}

//得到ESN。
ALM_INT32 Adapter_GetEsn(OUT ALM_ENV_ESN_STRU* pstEsn)
{
    memcpy(pstEsn, &s_stEsn, sizeof(s_stEsn));

    return ALM_OK;
}

//实现持久性写。
ALM_INT32 Adapter_PsIOWrite(CONST ALM_VOID* pBuf,
    IN ALM_UINT32 uiLen, 
    IN ALM_UINT32 uiOffSet)
{
    FILE *pFile = ALM_NULL;
    long iRet;
    const char* pcFileName = PS_FILE;

    if (ALM_NULL == pBuf || 0 == uiLen)
        return ALM_ERR_UNKNOW;
    
    
    /***********************************************************
    * 选择并验证文件有效性。
    ***********************************************************/
    if (!Util_FileChk(pcFileName, true))
    {
        STF_printLog("WRITE回调: 文件%s不存在。", pcFileName);
        goto LAB_ERR;
    }

    /***********************************************************
    * 打开文件并定位文件到写的偏移位。
    ***********************************************************/
    pFile = fopen(pcFileName, "rb+");
    if (ALM_NULL == pFile)
    {
        STF_printLog("打开文件%s失败。", pcFileName);
        goto LAB_ERR;
    }

    iRet = fseek(pFile, uiOffSet, SEEK_SET);
    if (iRet != 0)
    {
        STF_printLog("写入数据到持久化文件时定位到偏移位失败。");
        goto LAB_ERR;
    }

    /***********************************************************
    * 写入数据。
    ***********************************************************/
    iRet = fwrite(pBuf,1, uiLen, pFile);
    if (iRet != uiLen)
    {
        STF_printLog("写入数据到持久化文件失败。");
        goto LAB_ERR;
    }

    /***********************************************************
    * 关闭文件。
    ***********************************************************/
    iRet = fclose(pFile);
    if (iRet != 0)
        STF_printLog("关闭文件%s失败。", pcFileName);
    
    return ALM_OK;

LAB_ERR:
    if (ALM_NULL != pFile)
        fclose(pFile);

    //写入失败，会导致部分重要信息未能持久化。
    return ALM_ERR_UNKNOW;
}

//实现持久化读。
ALM_INT32 Adapter_PsIORead(OUT ALM_VOID *pBuf,
    INOUT ALM_UINT32 *puiLen,
    IN ALM_UINT32 uiOffSet)
{
    FILE* pFile = ALM_NULL;
    const char* pcFileName = PS_FILE;
    long lLenTmp;
    long lRet;
    bool bNeedCreate = true;

    if (ALM_NULL == pBuf || 0 == *puiLen)
        return ALM_ERR_UNKNOW;

    /***********************************************************
    * 选择并验证文件有效性。
    ***********************************************************/
    if (!Util_FileChk(pcFileName, bNeedCreate))
    {
        STF_printLog("READ回调: 文件%s不存在。", pcFileName);
        goto LAB_ERR;
    }
    /***********************************************************
    * 打开文件并定位到偏移位。
    ***********************************************************/
    pFile = fopen(pcFileName, "rb");
    if (ALM_NULL == pFile)
    {
        STF_printLog("打开文件%s失败。", pcFileName);
        goto LAB_ERR;
    }

    //需要进行偏移位有效性及读取数据长度有效性判断。
    fseek(pFile,0,SEEK_END);
    lLenTmp = ftell(pFile);

    //偏移有效性判断。
    if ((lLenTmp < 0) || ((long)uiOffSet > lLenTmp))
        goto LAB_ERR;

    //待读取长度有效性判断。
    if(0==lLenTmp)
    {
        *puiLen = 0;
        return ALM_OK;
    }
    else if (lLenTmp < (long)*puiLen)
    {
        goto LAB_ERR;
    }
    
    //定位到指定偏移位。
    lRet = fseek(pFile, uiOffSet, SEEK_SET);
    if (lRet != 0)
        goto LAB_ERR;

    /***********************************************************
    * 读取数据
    ***********************************************************/
    lRet = fread(pBuf,1,(long)*puiLen,pFile);
    if (lRet != (long)*puiLen)
        goto LAB_ERR;

    /***********************************************************
    * 关闭文件
    ***********************************************************/
    lRet = fclose(pFile);
    if (lRet != 0)
        STF_printLog("关闭文件%s失败。", pcFileName);

    return ALM_OK;

LAB_ERR:
    if (ALM_NULL != pFile)
        fclose(pFile);
    *puiLen = 0;

    //返回错误会导致调用函数错误。与未读取到内容有不同处理。
    return ALM_ERR_UNKNOW;
}

/* 清空保密文件 */
ALM_INT32 Adapter_PsCls()
{
    FILE *pFile = ALM_NULL;
    long iRet;
    const char* pcFileName = PS_FILE;    
    
    /***********************************************************
    * 选择并验证文件有效性。
    ***********************************************************/
    if (!Util_FileChk(pcFileName, true))
    {
        STF_printLog("WRITE回调: 文件%s不存在。", pcFileName);
        goto LAB_ERR;
    }

    /***********************************************************
    * 截断文件内容。
    ***********************************************************/
    pFile = fopen(pcFileName, "wb+");
    if (ALM_NULL == pFile)
    {
        STF_printLog("清空文件%s失败。", pcFileName);
        goto LAB_ERR;
    }

    /***********************************************************
    * 关闭文件。
    ***********************************************************/
    iRet = fclose(pFile);
    if (iRet != 0)
        STF_printLog("关闭文件%s失败。", pcFileName);
    
    return ALM_OK;

LAB_ERR:
    if (ALM_NULL != pFile)
        fclose(pFile);

    //写入失败，会导致部分重要信息未能持久化。
    return ALM_ERR_UNKNOW;
}

//打印日志。
ALM_INT32 Adapter_Log(ALM_ENUM(ALM_LOG_LEVEL_ENUM) eLevel, CONST ALM_CHAR* pcLogStr)
{
	switch (eLevel)
	{
	case ALM_LOG_LEVEL_TRACE:
		STF_printLog("[%s: TRACE] %s", OS_TimeSimulaStr(), pcLogStr);
		break;

	case ALM_LOG_LEVEL_DEBUG:
		STF_printLog("[%s: DEBUG] %s", OS_TimeSimulaStr(), pcLogStr);
		break;

	case ALM_LOG_LEVEL_WARNING:
		STF_printLog("[%s:  WARN] %s", OS_TimeSimulaStr(), pcLogStr);
		break;

	case ALM_LOG_LEVEL_ERROR:
		STF_printLog("[%s: ERROR] %s", OS_TimeSimulaStr(), pcLogStr);
		break;

	case ALM_LOG_LEVEL_INFO:
		STF_printLog("[%s: INFO] %s", OS_TimeSimulaStr(), pcLogStr);
		break;

	default:
		return ALM_ERR_UNKNOW;
	}

	return ALM_OK;
}



ALM_INT32 Adapter_Alarm(CONST ALM_ALARM_PRD_CHANGE_STRU* pstArray,
    ALM_UINT32 uiNum)
{
    ALM_UINT32 uiIdx;
    ALM_UINT32 uiIdx2;
    CONST ALM_ALARM_PRD_CHANGE_STRU* pstAlarmChangeTem = ALM_NULL;
    ALM_ALARM_LK_STRU* pstRecoveryLkAlarmTem = ALM_NULL;
    ALM_ALARM_LK_STRU* pstFaultLkAlarmTem = ALM_NULL;
    ALM_ALARM_BASE_STRU* pstEmAlarmTem = ALM_NULL;
    ALM_ALARM_BASE_STRU* pstStickAlarmTem = ALM_NULL;
    ALM_ALARM_FEATURE_STRU* pstFeatureAlarmTem = ALM_NULL;
    ALM_ALARM_BASE_FEATURE_STRU* pstBaseFeatureTem = ALM_NULL;

#define RSN_MAX_LEN 256 

	ALM_CHAR acRsn[RSN_MAX_LEN + 1] = {0};

    for (uiIdx = 0; uiIdx < uiNum; uiIdx++)
    {	
		pstAlarmChangeTem = pstArray + uiIdx;

        if (ALM_NULL != pstAlarmChangeTem)
        {
            /*打印告警的产品状态*/
            STF_printLog("**************** Alarm Prd  ***********\n");
            STF_printLog("id: %d\n",pstAlarmChangeTem->iPrdId);
            STF_printLog("id: %d\n",pstAlarmChangeTem->pstPrdState->iPrdId);
            STF_printLog("EnterTime: %s\n",pstAlarmChangeTem->pstPrdState->stBase.acEnterTime);
            STF_printLog("Lsn: %s\n",pstAlarmChangeTem->pstPrdState->stBase.acLsn);
            STF_printLog("Perm: %d\n",pstAlarmChangeTem->pstPrdState->stBase.bPerm);
            STF_printLog("State: %s\n",Tl_AlmPrdState2Str(pstAlarmChangeTem->pstPrdState->stBase.eState));
            STF_printLog("LeftTime: %s\n",Tl_AlmMins2DayStr(pstAlarmChangeTem->pstPrdState->stBase.uiLeftMins));
            STF_printLog("**************** Alarm Prd  ***********\n\n");

            /*打印告警文件状态*/
            if (ALM_NULL != &pstAlarmChangeTem->stAlarm.stLkAlarm)
            {
                if (ALM_NULL != pstAlarmChangeTem->stAlarm.stLkAlarm.pstRecoveryLkAlarm)
                {
                    pstRecoveryLkAlarmTem = pstAlarmChangeTem->stAlarm.stLkAlarm.pstRecoveryLkAlarm;
					memcpy_s(&g_stRecoveryLkAlarmTem,sizeof(ALM_ALARM_LK_STRU),pstRecoveryLkAlarmTem,sizeof(ALM_ALARM_LK_STRU));

                    STF_printLog("***************** Lic_Recovery **********\n");
					STF_printLog("acGraceEnterTime: %s\n",pstRecoveryLkAlarmTem->acGraceEnterTime);
                    STF_printLog("acGraceDeadLine: %s\n",pstRecoveryLkAlarmTem->acGraceDeadLine);
                    STF_printLog("AlarmType: %s\n",Tl_AlmAlarmType2Str(pstRecoveryLkAlarmTem->stBaseAlarm.eAlarmType));
                    STF_printLog("AlarmName: %s\n",Tl_AlmAlarmName2Str(pstRecoveryLkAlarmTem->stBaseAlarm.eAlarmName));
                    STF_printLog("AlarmLevel: %d\n",pstRecoveryLkAlarmTem->stBaseAlarm.eAlarmLevel);
                    STF_printLog("AlarmReason: %s\n",
						Tl_AlmAlarmRsnToStr(pstRecoveryLkAlarmTem->stBaseAlarm.ulAlarmReason,acRsn,RSN_MAX_LEN+1));
                    STF_printLog("State: %s\n",Tl_AlmState2Str(pstRecoveryLkAlarmTem->stBaseAlarm.eState));
                    STF_printLog("EnterTime: %s\n",pstRecoveryLkAlarmTem->stBaseAlarm.acStateStartTime);
                    STF_printLog("LeftTime: %s\n",Tl_AlmMins2DayStr(pstRecoveryLkAlarmTem->stBaseAlarm.uiStateLeftMins));
                    STF_printLog("Lsn: %s\n",pstRecoveryLkAlarmTem->stBaseAlarm.acLsn);
                    STF_printLog("***************** Lic_Recovery **********\n\n");


                }

                if (ALM_NULL != pstAlarmChangeTem->stAlarm.stLkAlarm.pstFaultLkAlarm)
                {
                    pstFaultLkAlarmTem = pstAlarmChangeTem->stAlarm.stLkAlarm.pstFaultLkAlarm;
					memcpy_s(&g_stFaultLkAlarmTem,sizeof(ALM_ALARM_LK_STRU),pstFaultLkAlarmTem,sizeof(ALM_ALARM_LK_STRU));

                    STF_printLog("***************** Lic_Fault **********\n");
					STF_printLog("acGraceEnterTime: %s\n",pstFaultLkAlarmTem->acGraceEnterTime);
                    STF_printLog("acGraceDeadLine: %s\n",pstFaultLkAlarmTem->acGraceDeadLine);
                    STF_printLog("AlarmType: %s\n",Tl_AlmAlarmType2Str(pstFaultLkAlarmTem->stBaseAlarm.eAlarmType));
                    STF_printLog("AlarmName: %s\n",Tl_AlmAlarmName2Str(pstFaultLkAlarmTem->stBaseAlarm.eAlarmName));
                    STF_printLog("AlarmLevel: %d\n",pstFaultLkAlarmTem->stBaseAlarm.eAlarmLevel);
					STF_printLog("AlarmReason: %s\n",
						Tl_AlmAlarmRsnToStr(pstFaultLkAlarmTem->stBaseAlarm.ulAlarmReason,acRsn,RSN_MAX_LEN+1));
                    STF_printLog("State: %s\n",Tl_AlmState2Str(pstFaultLkAlarmTem->stBaseAlarm.eState));
                    STF_printLog("EnterTime: %s\n",pstFaultLkAlarmTem->stBaseAlarm.acStateStartTime);
                    STF_printLog("LeftTime: %s\n",Tl_AlmMins2DayStr(pstFaultLkAlarmTem->stBaseAlarm.uiStateLeftMins));
                    STF_printLog("Lsn: %s\n",pstFaultLkAlarmTem->stBaseAlarm.acLsn);
                    STF_printLog("***************** Lic_Fault **********\n");

                }
            }

            if (ALM_NULL != pstAlarmChangeTem->stAlarm.pstEmAlarm)
            {
                pstEmAlarmTem = pstAlarmChangeTem->stAlarm.pstEmAlarm;
				memcpy_s(&g_stEmAlarmTem,sizeof(ALM_ALARM_BASE_STRU),pstEmAlarmTem,sizeof(ALM_ALARM_BASE_STRU));

                STF_printLog("***************** EM **********\n");
                STF_printLog("AlarmType: %s\n",Tl_AlmAlarmType2Str(pstEmAlarmTem->eAlarmType));
                STF_printLog("AlarmName: %s\n",Tl_AlmAlarmName2Str(pstEmAlarmTem->eAlarmName));
                STF_printLog("AlarmLevel: %d\n",pstEmAlarmTem->eAlarmLevel);
				STF_printLog("AlarmReason: %s\n",
					Tl_AlmAlarmRsnToStr(pstEmAlarmTem->ulAlarmReason,acRsn,RSN_MAX_LEN+1));
                STF_printLog("State: %s\n",Tl_AlmState2Str(pstEmAlarmTem->eState));
                STF_printLog("EnterTime: %s\n",pstEmAlarmTem->acStateStartTime);
                STF_printLog("LeftTime: %s\n",Tl_AlmMins2DayStr(pstEmAlarmTem->uiStateLeftMins));
                STF_printLog("Lsn: %s\n",pstEmAlarmTem->acLsn);
                STF_printLog("***************** EM **********\n\n");
            }

            if (ALM_NULL != pstAlarmChangeTem->stAlarm.pstStickAlarm)
            {
                pstStickAlarmTem = pstAlarmChangeTem->stAlarm.pstStickAlarm;
				memcpy_s(&g_stStickAlarmTem,sizeof(ALM_ALARM_BASE_STRU),pstStickAlarmTem,sizeof(ALM_ALARM_BASE_STRU));

                STF_printLog("***************** STICK **********\n");
                STF_printLog("AlarmType: %s\n",Tl_AlmAlarmType2Str(pstStickAlarmTem->eAlarmType));
                STF_printLog("AlarmName: %s\n",Tl_AlmAlarmName2Str(pstStickAlarmTem->eAlarmName));
                STF_printLog("AlarmLevel: %d\n",pstStickAlarmTem->eAlarmLevel);
				STF_printLog("AlarmReason: %s\n",
					Tl_AlmAlarmRsnToStr(pstStickAlarmTem->ulAlarmReason,acRsn,RSN_MAX_LEN+1));
                STF_printLog("State: %s\n",Tl_AlmState2Str(pstStickAlarmTem->eState));
                STF_printLog("EnterTime: %s\n",pstStickAlarmTem->acStateStartTime);
                STF_printLog("LeftTime: %s\n",Tl_AlmMins2DayStr(pstStickAlarmTem->uiStateLeftMins));
                STF_printLog("Lsn: %s\n",pstStickAlarmTem->acLsn);
                STF_printLog("***************** STICK **********\n\n");
            }


            if (ALM_NULL != pstAlarmChangeTem->stAlarm.pstFeatureAlarms)
            {
                for (uiIdx2 = 0; uiIdx2 < pstAlarmChangeTem->stAlarm.uiFeatureNum; uiIdx2++)
                {
                    pstFeatureAlarmTem = pstAlarmChangeTem->stAlarm.pstFeatureAlarms + uiIdx2;

					/*拷贝的为最后一个feature的全部数据*/
					memcpy_s(&g_stFeatureAlarmTem,sizeof(ALM_ALARM_FEATURE_STRU),pstFeatureAlarmTem,sizeof(ALM_ALARM_FEATURE_STRU));

                        if (ALM_NULL != pstFeatureAlarmTem->pstRecoveryFeature)
                        {
                            pstBaseFeatureTem = pstFeatureAlarmTem->pstRecoveryFeature;
							memcpy_s(&g_stBaseFeatureRec,sizeof(ALM_ALARM_BASE_FEATURE_STRU),pstBaseFeatureTem,sizeof(ALM_ALARM_BASE_FEATURE_STRU));

                            STF_printLog("***************** FEATURE_RECOVERY_START**********\n");
                            STF_printLog("Feature Name: %s\n",pstFeatureAlarmTem->acFeatureName);
                            STF_printLog("Feature DeadLine: %s\n",pstFeatureAlarmTem->acFeatureDeadLine);
                            STF_printLog("Feature ID: %d\n",pstFeatureAlarmTem->uiFeatureId);
                            STF_printLog("AlarmType: %s\n",Tl_AlmAlarmType2Str(pstBaseFeatureTem->eAlarmType));
                            STF_printLog("AlarmName: %s\n",Tl_AlmAlarmName2Str(pstBaseFeatureTem->eAlarmName));
                            STF_printLog("AlarmLevel: %d\n",pstBaseFeatureTem->eAlarmLevel);
							STF_printLog("AlarmReason: %s\n",
								Tl_AlmAlarmRsnToStr(pstBaseFeatureTem->ulAlarmReason,acRsn,RSN_MAX_LEN+1));
                            STF_printLog("State: %s\n",Tl_AlmState2Str(pstBaseFeatureTem->eFeatureState));
                            STF_printLog("LeftTime: %s\n",Tl_AlmMins2DayStr(pstBaseFeatureTem->uiStateLeftMins));
                            STF_printLog("acLsn: %s\n",pstBaseFeatureTem->acLsn);
                            
                            STF_printLog("***************** FEATURE_RECOVERY_END **********\n\n");

                        }

                        if (ALM_NULL != pstFeatureAlarmTem->pstFaultFeature)
                        {
                            pstBaseFeatureTem = pstFeatureAlarmTem->pstFaultFeature;
							memcpy_s(&g_stBaseFeatureFault,sizeof(ALM_ALARM_BASE_FEATURE_STRU),pstBaseFeatureTem,sizeof(ALM_ALARM_BASE_FEATURE_STRU));

                            STF_printLog("***************** FEATURE_FAULT **********\n");
                            STF_printLog("Feature Name: %s\n",pstFeatureAlarmTem->acFeatureName);
                            STF_printLog("Feature DeadLine: %s\n",pstFeatureAlarmTem->acFeatureDeadLine);
                            STF_printLog("Feature ID: %d\n",pstFeatureAlarmTem->uiFeatureId);
                            STF_printLog("AlarmType: %s\n",Tl_AlmAlarmType2Str(pstBaseFeatureTem->eAlarmType));
                            STF_printLog("AlarmName: %s\n",Tl_AlmAlarmName2Str(pstBaseFeatureTem->eAlarmName));
                            STF_printLog("AlarmLevel: %d\n",pstBaseFeatureTem->eAlarmLevel);
							STF_printLog("AlarmReason: %s\n",
								Tl_AlmAlarmRsnToStr(pstBaseFeatureTem->ulAlarmReason,acRsn,RSN_MAX_LEN+1));
                            STF_printLog("State: %s\n",Tl_AlmState2Str(pstBaseFeatureTem->eFeatureState));
                            STF_printLog("LeftTime: %s\n",Tl_AlmMins2DayStr(pstBaseFeatureTem->uiStateLeftMins));
							STF_printLog("acLsn: %s\n",pstBaseFeatureTem->acLsn);
                            STF_printLog("***************** FEATURE_FAULT **********\n\n");
                        }

                    }

            } 
        }
    }

	return ALM_OK;
}


/* 状态变更通知 */
ALM_INT32 Adapter_NotifyChange(CONST ALM_NOTIFY_CHANGES_STRU* pstChanges)
{
    ALM_UINT32 uiIdx;
    ALM_UINT32 uiIdx2;
    CONST ALM_NOTIFY_CHANGE_STRU* pstPrd;
    ALM_BOOL bTmp;

#define RSN_MAX_LEN 256 
    
    ALM_CHAR acRsn[RSN_MAX_LEN + 1] = {0};

    if (ALM_NULL == pstChanges && 0 == pstChanges->uiNum)
    {
        return ALM_OK;
    }
   
#define ALMRsn2Str_M(ulRsn) \
    ALMDbgRsnAndOptToRsnStr(pstPrd->eOperate, ulRsn, acRsn, sizeof(acRsn))

/* License状态转换为字符串, ALM_STATE_LIC_STRU */
#define ALMLicState2Str_M(stLicState) \
    stLicState.bUsing ? Tl_AlmState2Str(stLicState.stBase.eState) : "NotUsing"
    
    STF_printLog("\n%s:%s\n", "Notify_Change_Start", OS_TimeSimulaStr());

    /* 打印产品状态变更。 */
    for (bTmp = ALM_FALSE, uiIdx = 0; uiIdx < pstChanges->uiNum; ++uiIdx)
    {
        pstPrd = &pstChanges->pstArray[uiIdx];

        if (pstPrd->pstPrd != ALM_NULL)
        {
            /* 打印头 */
            if (!bTmp)
            {
                STF_printLog("\n\t%s\n", "Notify: Prd_State_Change");

                STF_printLog("\t%-10s %-15s %-15s %-20s %-5s %s\n",
                    "PrdId", "Old", "New", "enter",
                    "perm", "left");

                STF_printLog("\t%-10s %-15s %-15s %-20s %-5s %s\n",
                    "----------", "---------------","---------------",
                    "--------------------",
                    "-----", "---------------");
                
                bTmp = ALM_TRUE;
            }

            /* 打印条目 */
            STF_printLog("\t%-10d %-15s %-15s %-20s %-5s %-15s\n",
                pstPrd->iPrdId,
                Tl_AlmPrdState2Str(pstPrd->pstPrd->stOld.eState),
                Tl_AlmPrdState2Str(pstPrd->pstPrd->stNew.eState),
                pstPrd->pstPrd->stNew.acEnterTime,
                pstPrd->pstPrd->stNew.bPerm ? "true" : "not",
                Tl_AlmMins2DayStr(pstPrd->pstPrd->stNew.uiLeftMins));
        }
    }

    /* 打印BBOM状态变更 */
    for (bTmp = ALM_FALSE, uiIdx = 0; uiIdx < pstChanges->uiNum; ++uiIdx)
    {
        pstPrd = &pstChanges->pstArray[uiIdx];

        for (uiIdx2 = 0; uiIdx2 < pstPrd->stBboms.uiNum; ++uiIdx2)
        {
            if (!pstPrd->stBboms.pstArray[uiIdx2].bStateChange)
            {
                continue;
            }
        
            /* 打印头 */
            if (!bTmp)
            {
                STF_printLog("\n\t%s\n", "Notify: Bbom_State_Change");

                STF_printLog("\t%-10s %-20s %-10s %-15s %-15s %-20s %-5s %s\n",
                    "PrdId", "Name", "Id", "Old", "New", "enter",
                    "perm", "left");

                STF_printLog("\t%-10s %-20s %-10s %-15s %-15s %-20s %-5s %s\n",
                    "----------", "--------------------", "----------",
                    "---------------", "---------------",
                    "--------------------", "-----",
                    "-------------------------");
                bTmp = ALM_TRUE;
            }

            /* 打印条目 */
            STF_printLog("\t%-10d %-20s %-10d %-15s %-15s %-20s %-5s %-15s\n",
                pstPrd->iPrdId,
                pstPrd->stBboms.pstArray[uiIdx2].acName,
                pstPrd->stBboms.pstArray[uiIdx2].iBbomId,
                Tl_AlmPrdState2Str(pstPrd->stBboms.pstArray[uiIdx2].stOld.stBase.eState),
                Tl_AlmPrdState2Str(pstPrd->stBboms.pstArray[uiIdx2].stNew.stBase.eState),
                pstPrd->stBboms.pstArray[uiIdx2].stNew.stBase.acEnterTime,
                pstPrd->stBboms.pstArray[uiIdx2].stNew.stBase.bPerm ? "true" : "not",
                Tl_AlmMins2DayStr(pstPrd->stBboms.pstArray[uiIdx2].stNew.stBase.uiLeftMins));
        }
    }

    /* 打印BBOM值变更 */
    for (bTmp = ALM_FALSE, uiIdx = 0; uiIdx < pstChanges->uiNum; ++uiIdx)
    {
        pstPrd = &pstChanges->pstArray[uiIdx];

        for (uiIdx2 = 0; uiIdx2 < pstPrd->stBboms.uiNum; ++uiIdx2)
        {
            if (!pstPrd->stBboms.pstArray[uiIdx2].bValueChange)
            {
                continue;
            }

            /* 打印头 */
            if (!bTmp)
            {
                STF_printLog("\n\t%s\n", "Notify: Bbom_Value_Change");

                STF_printLog("\t%-10s %-20s %-10s %-10s %-10s\n",
                    "PrdId", "Name", "Id", "OldVal", "NewVal");

                STF_printLog("\t%-10s %-20s %-10s %-10s %-10s\n",
                    "----------", "--------------------",
                    "----------", "----------", "----------");
                bTmp = ALM_TRUE;
            }

            /* 打印条目 */
            STF_printLog("\t%-10d %-20s %-10d %-10llu %-10llu\n",
                pstPrd->iPrdId,
                pstPrd->stBboms.pstArray[uiIdx2].acName,
                pstPrd->stBboms.pstArray[uiIdx2].iBbomId,
                pstPrd->stBboms.pstArray[uiIdx2].stOld.ulVal,
                pstPrd->stBboms.pstArray[uiIdx2].stNew.ulVal);
        }
    }
    
    /* 打印License状态变更 */
    for (bTmp = ALM_FALSE, uiIdx = 0; uiIdx < pstChanges->uiNum; ++uiIdx)
    {
        pstPrd = &pstChanges->pstArray[uiIdx];

        for (uiIdx2 = 0; uiIdx2 < pstPrd->stLics.uiNum; ++uiIdx2)
        {
            if (!pstPrd->stLics.pstArray[uiIdx2].bStateChange)
            {
                continue;
            }
            /* 打印头 */
            if (!bTmp)
            {
                STF_printLog("\n\t%s\n", "Notify: Lic_State_Change");

                STF_printLog("\t%-10s %-20s %-5s %-15s %-15s %-10s %-20s %-5s %-15s\n",
                    "PrdId", "Lsn", "Type", "Old", "New", "Recover", "Enter",
                    "Perm", "Left");

                STF_printLog("\t%-10s %-20s %-5s %-15s %-15s %-10s %-20s %-5s %s\n",
                    "----------", "--------------------", "-----",
                    "---------------", "---------------",
                    "----------", "--------------------",
                    "-----", "---------------");
                bTmp = ALM_TRUE;
            }

            /* 打印条目 */
            STF_printLog("\t%-10d %-20s %-5s %-15s %-15s %-10s %-20s %-5s %-15s <-- Rsn:%s\n",
                pstPrd->iPrdId,
                pstPrd->stLics.pstArray[uiIdx2].acLsn,
                Tl_AlmLicType2Str(pstPrd->stLics.pstArray[uiIdx2].eLicType),
                ALMLicState2Str_M(pstPrd->stLics.pstArray[uiIdx2].stOld),
                ALMLicState2Str_M(pstPrd->stLics.pstArray[uiIdx2].stNew),
                pstPrd->stLics.pstArray[uiIdx2].bRecoverUsing ? "yes" : "not",
                pstPrd->stLics.pstArray[uiIdx2].stNew.stBase.acEnterTime,
                pstPrd->stLics.pstArray[uiIdx2].stNew.stBase.bPerm ? "true" : "false",
                Tl_AlmMins2DayStr(pstPrd->stLics.pstArray[uiIdx2].stNew.stBase.uiLeftMins),
                ALMRsn2Str_M(pstPrd->stLics.pstArray[uiIdx2].stNew.stBase.ulStateRsn));
        }
    }

    /* 打印License校验项 */
    for (bTmp = ALM_FALSE, uiIdx = 0; uiIdx < pstChanges->uiNum; ++uiIdx)
    {
        pstPrd = &pstChanges->pstArray[uiIdx];

        for (bTmp = ALM_FALSE, uiIdx2 = 0; uiIdx2 < pstPrd->stLics.uiNum; ++uiIdx2)
        {
            if (pstPrd->stLics.pstArray[uiIdx2].bVerifyChange)
            {
                /* 打印头 */
                if (!bTmp)
                {
                    STF_printLog("\n\t%s\n", "Notify: Lic_Verify_Change");
                    STF_printLog("\t%-10s %-20s\n\t%-10s %-20s\n", "PrdId", "Lsn",
                        "----------",  "--------------------");
                    bTmp = ALM_TRUE;
                }

                /* 打印条目 */
                STF_printLog("\t%-10d %-20s <-- Rsn:%s ",
                    pstPrd->iPrdId,
                    pstPrd->stLics.pstArray[uiIdx2].acLsn,
                    ALMRsn2Str_M(pstPrd->stLics.pstArray[uiIdx2].stNew.stBase.ulStateRsn));
            }
        }
    }
    
    /* 打印License Bbom状态变更 */
    for (bTmp = ALM_FALSE, uiIdx = 0; uiIdx < pstChanges->uiNum; ++uiIdx)
    {
        pstPrd = &pstChanges->pstArray[uiIdx];

        for (uiIdx2 = 0; uiIdx2 < pstPrd->stLicBboms.uiNum; ++uiIdx2)
        {
            /* 打印头 */
            if (!bTmp)
            {
                STF_printLog("\n\t%s\n", "Notify: Lic_Bbom_State_Change");

                STF_printLog("\t%-10s %-10s %-20s %-6s %-10s %-10s %-15s\n"
                    "\t%-10s %-10s %-20s %-6s %-10s %-10s %-15s\n",
                    "PrdId", "BbomId", "BbomName", "SameId", "OldState",
                    "NewState", "Left",
                    "----------", "----------", "--------------------",
                    "------", "----------", "----------", "---------------");

                bTmp = ALM_TRUE;
            }

            /* 打印条目 */
            STF_printLog("\t%-10d %-10d %-20s %-6d %-10s %-10s %-15s <-- Rsn:%s\n",
                pstPrd->iPrdId,
                pstPrd->stLicBboms.pstArray[uiIdx2].iBbomId,
                pstPrd->stLicBboms.pstArray[uiIdx2].pstBbomInfo->acBBomName,
                pstPrd->stLicBboms.pstArray[uiIdx2].pstBbomInfo->uiSameNameId,
                Tl_AlmState2Str(pstPrd->stLicBboms.pstArray[uiIdx2].stOld.eState),
                Tl_AlmState2Str(pstPrd->stLicBboms.pstArray[uiIdx2].stNew.eState),
                Tl_AlmMins2DayStr(pstPrd->stLicBboms.pstArray[uiIdx2].stNew.uiLeftMins),
                ALMRsn2Str_M(pstPrd->stLicBboms.pstArray[uiIdx2].stNew.ulStateRsn));
        }
    }

    STF_printLog("\n%s\n\n\n", "Notify_Change_End");

    return ALM_OK;
}


/* 将原因附加操作转换为原因字符串 */
static CONST ALM_CHAR* ALMDbgRsnAndOptToRsnStr(
    ALM_ENUM(ALM_STATE_OPERATE_ENUM) eOperate,
    ALM_UINT64 ulRsn,
    OUT ALM_CHAR* pcRsn,
    ALM_UINT32 uiRsnSize)
{
    if (ALM_NULL == pcRsn || 0 == uiRsnSize)
    {
        return ALM_NULL;
    }

    Tl_AlmRsnToStr(ulRsn, pcRsn, uiRsnSize);

    (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize,  ", by ");
    switch (eOperate)
    {
        /* 激活License */
    case ALM_STATE_OPERATE_ACTIVE_LIC:
        (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "active license");
        break;

        /* 失效License文件 */
    case ALM_STATE_OPERATE_REVOKE_LIC:
        (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "revoke license");
        break;

        /* 启动紧急 */
    case ALM_STATE_OPERATE_START_EMERGENCY:
        (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "start emergency");
        break;

        /* 停止紧急 */
    case ALM_STATE_OPERATE_STOP_EMERGENCY:
        (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "stop emergency");
        break;

        /* 启动Stick */
    case ALM_STATE_OPERATE_START_STICK:
        (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "start stick");
        break;

        /* 停止Stick */
    case ALM_STATE_OPERATE_STOP_STICK:
        (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "stop stick");
        break;

        /* 执行恢复动作 */
    case ALM_STATE_OPERATE_RECOVER:
        (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "recover");
        break;

        /* 刷新状态 */
    case ALM_STATE_OPERATE_REFRESH:
        (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "refresh");
        break;

    case ALM_STATE_OPERATE_DISABLE:
        (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "disable");
        break;

    default:
        (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "unknow operator");
        break;
    }

    (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, ".");

    return pcRsn;
}
