/*******************************************************************************
Description    : 
History        :
<version> <date>     <Author> <Description>
*******************************************************************************/
#include "test_common.h"
#include "alm_tool_base.h"
#include "alm_table_tool.h"
#include "alm_os.h"
#include "alm_state_mgr.h"
#include "alm_prd_mgr.h"
#include "alm_prd_store.h"
#include "test_stru_lic.h"
#include "callback.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */


static char s_acStartTime[30];

//测试用的通用密钥。
char* g_pcKey = ALM_NULL;

ALM_VOID Test_Start()
{
    const char* pcKeyFile = TST_WORKPATH"license/other/key.txt";
    int iKeyLen;
    int iRet;

    //启动OS，初始化日志等。
    OS_Start();
    (void)init_log();
    Test_InitRegister();

    ALM_sprintf_s(s_acStartTime, sizeof(s_acStartTime), "%s", OS_DateStr());
    ALM_DbgSetPrint((ALM_DBG_PRINT_FUNC)STF_printLog);

    //初始化通用密钥
    TEST_FREE(&g_pcKey);
    iRet = Test_ReadFileToBuf(pcKeyFile, &g_pcKey, &iKeyLen);
    if (RET_OK != 0)
    {
        STF_printLog("Start failed.\n");
        abort();
    }
}

ALM_VOID Test_Stop()
{
    TEST_FREE(&g_pcKey);
    STF_printLog("Run from \"%s\" to \"%s\"\n", s_acStartTime, OS_DateStr());

    FRM_ClearTestcase();
    tstcleanup_log();
    OS_Close();
}

static ALM_INT32 TestLog(ALM_ENUM(ALM_LOG_LEVEL_ENUM) eLevel,
    CONST ALM_CHAR* pcLogStr)
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

ALM_INT32 Test_Init(ALM_BOOL bCLsPs, ALM_BOOL bFixHour, ALM_UINT8 ucFlashHour,
    ALM_UINT8 ucFlashInterval)
{
    ALM_INT32 iRet;
    ALM_ADAPTER_BASE_FUNCS_STRU stFunc = {0};
    ALM_ADAPTER_OTHER_FUNCS_STRU stOtherFuncs = {0};
    ALM_STATE_MGR_CFG_STRU stAutoCheck = {bFixHour, ucFlashHour, ucFlashInterval};
    ALM_START_CFG_STRU stStartCfg = {ALM_CONST_LIC_CBB_C_VER};

    /* 清空保密文件 */
    if (bCLsPs)
    {
        Adapter_PsCls();
    }
    
    /************************************************************************
    * 设置回调
    ************************************************************************/
    stFunc.fnGetTime = (ALM_ADAPTER_BASE_GET_TIME_FUNC)OS_DateSimulaGet;
    stFunc.fnMemFree = OS_MemAlmFree;
    stFunc.fnMemMalloc = OS_MemAlmAlloc;
    stFunc.fnMutexAcquire = OS_LockP;
    stFunc.fnMutexCreate = OS_LockCreate;
    stFunc.fnMutexDel = OS_LockDelete;
    stFunc.fnMutexRelease = OS_LockV;
    stFunc.fnTimerStart = OS_TimerNew;
    stFunc.fnTimerStop = OS_TimerDel;
    stFunc.fnGetEsn = Adapter_GetEsn;
    stFunc.fnPsRead = Adapter_PsIORead;
    stFunc.fnPsWrite = Adapter_PsIOWrite;
    
    iRet = ALM_AdapterBaseFuncs(&stFunc);
    STF_ASSERT_LAB(ALM_OK == iRet, LAB_ERR);
    	
    stOtherFuncs.fnAlarm = Adapter_Alarm;
    stOtherFuncs.fnLog = TestLog;
    stOtherFuncs.fnNotify = Adapter_NotifyChange;
    
    iRet = ALM_AdapterOtherFuncs(&stOtherFuncs);
    STF_ASSERT_LAB(ALM_OK == iRet, LAB_ERR);

    iRet = ALM_LogSetLevel(ALM_LOG_LEVEL_ERROR);
    STF_ASSERT_LAB(ALM_OK == iRet, LAB_ERR);


    /************************************************************************
    * 初始化及配置。
    ************************************************************************/

    /* 初始化 */
    iRet = ALM_Start(&stStartCfg);
    STF_ASSERT_LAB(ALM_OK == iRet, LAB_ERR);

    ALM_SafeTimeSetBaseTime("1900-01-01 00:00:00", 0);

    /* 配置自动校验 */
    iRet = ALM_StateMgrCfgAutoRefresh(&stAutoCheck);
    STF_ASSERT_LAB(ALM_OK == iRet, LAB_ERR);
    
    return ALM_OK;

LAB_ERR:
    ALM_LogInfo_M(("init fail."));
    return iRet;
}

ALM_VOID Test_Cls()
{
    ALM_INT32 iRet;

    iRet = ALM_Stop();
    ALM_CallVoid_M((ALM_NULL));
}


/*打印附加信息*/
ALM_VOID PrintAdditon(ALM_LIC_STORE_ADDITION_STRU *pstAddtion)
{

    STF_printLog("acFileCopyRight:%s\n",pstAddtion->acFileCopyRight);
    STF_printLog("acFileFormatVersion:%s\n",pstAddtion->acFileFormatVersion);
    STF_printLog("acFileCreator:%s\n",pstAddtion->acFileCreator);
    STF_printLog("acFileIssuer:%s\n",pstAddtion->acFileIssuer);
    STF_printLog("acFileCreateTime:%s\n",pstAddtion->acFileCreateTime);
    STF_printLog("acSwUpgradeDueDate:%s\n",pstAddtion->acSwUpgradeDueDate);
    STF_printLog("acFileCountry:%s\n",pstAddtion->acFileCountry);
    STF_printLog("acFileOffice:%s\n",pstAddtion->acFileOffice);
    STF_printLog("acFileCustom:%s\n",pstAddtion->acFileCustom);

    STF_printLog("uiLkNodeSequense:%d\n",pstAddtion->uiLkNodeSequense);
    STF_printLog("eLkLockModel:%d\n",pstAddtion->eLkLockModel);

    STF_printLog("acFileNodeName:%s\n",pstAddtion->acFileNodeName);
    STF_printLog("acFileNodeDes:%s\n",pstAddtion->acFileNodeDes);


}
/*打印通用信息*/
ALM_VOID PrintGener(ALM_LIC_STORE_GENERAL_STRU *pstGener)
{
    ALM_UINT32 i;
    STF_printLog("acLsn:%s\n",pstGener->acLsn);
    STF_printLog("eLicType:%d\n",pstGener->eLicType);
    for(i=0;i<pstGener->stEsns.uiNum;i++)
    {
        STF_printLog("ESN%d:%s\n",i,pstGener->stEsns.pstArray[i].acEsn);
    }
    STF_printLog("uiGraceDays:%d\n",pstGener->uiGraceDays);
    STF_printLog("acFilePrdName:%s\n",pstGener->acFilePrdName);
    STF_printLog("acFilePrdVer:%s\n",pstGener->acFilePrdVer);
    STF_printLog("uiAllowCount:%d\n",pstGener->uiAllowCount);
    STF_printLog("acSoftId:%s\n",pstGener->acSoftId);

}
/*打印Offers信息*/
ALM_VOID PrintLicOffers(ALM_LIC_STORE_OFFERS_STRU* stOffersQuerryRslt)
{
    ALM_UINT32 i;
    ALM_UINT32 j;
    ALM_UINT32 k;

    for (i = 0; i < stOffersQuerryRslt->uiNum; i++)
    {
        STF_printLog("LicOffer[%d]\n", i);
        STF_printLog("OfferName:%s\n", stOffersQuerryRslt->pstArray[i].acOfferName);
        STF_printLog("OfferVer:%s\n", stOffersQuerryRslt->pstArray[i].acOfferVer);

        for (j = 0 ; j < stOffersQuerryRslt->pstArray[i].stSboms.uiNum; j++)
        {
            STF_printLog("SBomCHDesc:%s\n", stOffersQuerryRslt->pstArray[i].stSboms.pstArray[j].acChsDesc);
            STF_printLog("SBomChsUnit:%s\n", stOffersQuerryRslt->pstArray[i].stSboms.pstArray[j].acChsUnit);
            STF_printLog("SBomENDesc:%s\n", stOffersQuerryRslt->pstArray[i].stSboms.pstArray[j].acEngDesc);
            STF_printLog("SBomEngUnit:%s\n", stOffersQuerryRslt->pstArray[i].stSboms.pstArray[j].acEngUnit);
            STF_printLog("SBomName:%s\n", stOffersQuerryRslt->pstArray[i].stSboms.pstArray[j].acName);

            STF_printLog("Sbom_Values:\n");

            for (k = 0; k < stOffersQuerryRslt->pstArray[i].stSboms.pstArray[j].stValues.uiNum; k++)
            {
                STF_printLog("SBomValues_%d_Value:%d\n", k, stOffersQuerryRslt->pstArray[i].stSboms.pstArray[j].stValues.pstArray[k].ulVal);
                STF_printLog("SBomValues_%d_Deadline:%s\n", k, stOffersQuerryRslt->pstArray[i].stSboms.pstArray[j].stValues.pstArray[k].acDeadline);
                STF_printLog("SBomValues_%d_IsNoControl:%d\n", k, stOffersQuerryRslt->pstArray[i].stSboms.pstArray[j].stValues.pstArray[k].bIsnoControl);
            }

            STF_printLog("Sbom_Bboms:\n");

            for (k = 0; k < stOffersQuerryRslt->pstArray[i].stSboms.pstArray[j].stS2Bboms.uiNum; k++)
            {
                STF_printLog("BBomName:%s\n", stOffersQuerryRslt->pstArray[i].stSboms.pstArray[j].stS2Bboms.pstArray[k].stBbom.acBBomName);
                STF_printLog("DeadLine:%s\n", stOffersQuerryRslt->pstArray[i].stSboms.pstArray[j].stS2Bboms.pstArray[k].stBbom.stTimeInfo.stValid.acDeadLine);
                STF_printLog("GroupName:%s\n", stOffersQuerryRslt->pstArray[i].stSboms.pstArray[j].stS2Bboms.pstArray[k].stBbom.acGroupName);
                STF_printLog("Type:%d\n", stOffersQuerryRslt->pstArray[i].stSboms.pstArray[j].stS2Bboms.pstArray[k].stBbom.eType);
                STF_printLog("AllowDays:%d\n", stOffersQuerryRslt->pstArray[i].stSboms.pstArray[j].stS2Bboms.pstArray[k].stBbom.stTimeInfo.stValid.uiRelDays);
                STF_printLog("GraceDays:%d\n", stOffersQuerryRslt->pstArray[i].stSboms.pstArray[j].stS2Bboms.pstArray[k].stBbom.stTimeInfo.uiGraceDays);
                STF_printLog("SameNameId:%d\n", stOffersQuerryRslt->pstArray[i].stSboms.pstArray[j].stS2Bboms.pstArray[k].stBbom.uiSameNameId);
                STF_printLog("Value:%d\n", stOffersQuerryRslt->pstArray[i].stSboms.pstArray[j].stS2Bboms.pstArray[k].stBbom.ulValue);
            }
        }

        for (j = 0 ; j < stOffersQuerryRslt->pstArray[i].stPackageItems.uiNum; j++)
        {
            STF_printLog("PackageItemCHDesc:%s\n", stOffersQuerryRslt->pstArray[i].stPackageItems.pstArray[j].acChsDesc);
            STF_printLog("PackageItemChsUnit:%s\n", stOffersQuerryRslt->pstArray[i].stPackageItems.pstArray[j].acChsUnit);
            STF_printLog("PackageItemENDesc:%s\n", stOffersQuerryRslt->pstArray[i].stPackageItems.pstArray[j].acEngDesc);
            STF_printLog("PackageItemEngUnit:%s\n", stOffersQuerryRslt->pstArray[i].stPackageItems.pstArray[j].acEngUnit);
            STF_printLog("PackageItemName:%s\n", stOffersQuerryRslt->pstArray[i].stPackageItems.pstArray[j].acName);

            STF_printLog("PackageItem_Values:\n");

            for (k = 0; k < stOffersQuerryRslt->pstArray[i].stPackageItems.pstArray[j].stValues.uiNum; k++)
            {
                STF_printLog("PackageItemValues_%d_Value:%d\n", k, stOffersQuerryRslt->pstArray[i].stPackageItems.pstArray[j].stValues.pstArray[k].ulVal);
                STF_printLog("PackageItemValues_%d_Deadline:%s\n", k, stOffersQuerryRslt->pstArray[i].stPackageItems.pstArray[j].stValues.pstArray[k].acDeadline);
            }

            STF_printLog("PackageItem_Sboms:\n");

            for (k = 0; k < stOffersQuerryRslt->pstArray[i].stPackageItems.pstArray[j].stP2Sboms.uiNum; k++)
            {
  /*              STF_printLog("BBomName:%s\n", stOffersQuerryRslt->pstOffers[i].stSboms.pstSboms[j].stS2Bboms.pstS2Bbom[k].stBbom.acBBomName);
                STF_printLog("DeadLine:%s\n", stOffersQuerryRslt->pstOffers[i].stSboms.pstSboms[j].stS2Bboms.pstS2Bbom[k].stBbom.stTimeInfo.stValid.acDeadLine);
                STF_printLog("GroupName:%s\n", stOffersQuerryRslt->pstOffers[i].stSboms.pstSboms[j].stS2Bboms.pstS2Bbom[k].stBbom.acGroupName);
                STF_printLog("Type:%d\n", stOffersQuerryRslt->pstOffers[i].stSboms.pstSboms[j].stS2Bboms.pstS2Bbom[k].stBbom.eType);
                STF_printLog("AllowDays:%d\n", stOffersQuerryRslt->pstOffers[i].stSboms.pstSboms[j].stS2Bboms.pstS2Bbom[k].stBbom.stTimeInfo.stValid.uiRelDays);
                STF_printLog("GraceDays:%d\n", stOffersQuerryRslt->pstOffers[i].stSboms.pstSboms[j].stS2Bboms.pstS2Bbom[k].stBbom.stTimeInfo.uiGraceDays);
                STF_printLog("SameNameId:%d\n", stOffersQuerryRslt->pstOffers[i].stSboms.pstSboms[j].stS2Bboms.pstS2Bbom[k].stBbom.uiSameNameId);
                STF_printLog("Value:%d\n", stOffersQuerryRslt->pstOffers[i].stSboms.pstSboms[j].stS2Bboms.pstS2Bbom[k].stBbom.ulValue);*/
            }
        }
    }

}

/* 添加测试产品 */
ALM_INT32 Test_AddPrd(ALM_INT32 iPrdId)
{
    ALM_INT32 iRet = ALM_OK;
    ALM_BOOL bExist;
    ALM_PRD_BBOM_STRU astBboms[] = {
        {0, TEST_LIC_BBOM_RES_NAME(0), ALM_LIC_BBOM_RESOURCE, 0},
        {1, TEST_LIC_BBOM_RES_NAME(1), ALM_LIC_BBOM_RESOURCE, 1},
        {2, TEST_LIC_BBOM_RES_NAME(2), ALM_LIC_BBOM_RESOURCE, 2},
        {3, TEST_LIC_BBOM_RES_NAME(3), ALM_LIC_BBOM_RESOURCE, 3},
        {4, TEST_LIC_BBOM_RES_NAME(4), ALM_LIC_BBOM_RESOURCE, 4},

        {5, TEST_LIC_BBOM_FUN_NAME(0), ALM_LIC_BBOM_FUNCTION, 0},
        {6, TEST_LIC_BBOM_FUN_NAME(1), ALM_LIC_BBOM_FUNCTION, 1},
        {7, TEST_LIC_BBOM_FUN_NAME(2), ALM_LIC_BBOM_FUNCTION, 1},
        {8, TEST_LIC_BBOM_FUN_NAME(3), ALM_LIC_BBOM_FUNCTION, 1},
        {9, TEST_LIC_BBOM_FUN_NAME(4), ALM_LIC_BBOM_FUNCTION, 1},
    };

    ALM_PRD_STRU stPrd = {
        {iPrdId, TEST_LIC_PRD_NAME, TEST_LIC_PRD_VER},
        {0,0,0},
        astBboms,
        sizeof(astBboms) / sizeof(ALM_PRD_BBOM_STRU),
        TEST_KEY
    };
    ALM_PRD_BASE_STRU stQuery;
    ALM_UINT32 uiFail;

    iRet = ALM_PrdMgrAdd(&stPrd, 1, &uiFail);
    STF_ASSERT_LAB(ALM_OK == iRet && 0 == uiFail, LAB_END);

    iRet = ALM_PrdStoreQuery(stPrd.stBase.iPrdId, &stQuery, &bExist);
    STF_ASSERT_LAB(ALM_OK == iRet, LAB_END);
    STF_ASSERT_LAB(bExist, LAB_END);

LAB_END:
    return iRet;
}

ALM_VOID PrintLicBboms(ALM_LIC_STORE_BBOMS_STRU *pstBboms)
{
    ALM_UINT32 i = 0;

    for(i = 0;i<pstBboms->uiNum;i++)
    {
        STF_printLog("LicBbom[%d]\n",i);
        STF_printLog("LicBbom count:%d\n",pstBboms->uiNum);
        STF_printLog("LicBbom GroupName:%s\n",pstBboms->pstArray[i].acGroupName);
        STF_printLog("Bbom Name:%s\n",pstBboms->pstArray[i].acBBomName);
        STF_printLog("Bbom Type:%d\n",pstBboms->pstArray[i].eType);
        STF_printLog("Bbom SameNameId:%d\n",pstBboms->pstArray[i].uiSameNameId);
        STF_printLog("Bbom Value:%d\n",pstBboms->pstArray[i].ulValue);
        STF_printLog("Bbom GraceDays:%d\n",pstBboms->pstArray[i].stTimeInfo.uiGraceDays);
        STF_printLog("Bbom :%s\n",pstBboms->pstArray[i].stTimeInfo.stValid.acDeadLine);
    }

}


//读取文件到缓存中，需要外部释放缓存內存。
int Test_ReadFileToBuf(const char* pcFile, OUT char** ppcBuf, OUT int* piReadLen)
{
    FILE* pLk = ALM_NULL;
    int iLkLen = 0;
    int iLen;

     //参数检测
    if (ALM_NULL == pcFile || ALM_NULL == ppcBuf)
    {
        STF_printLog("Null parameter!");
        return RET_ERR;
    }

    //初始化输出
    *ppcBuf = ALM_NULL;

    if (ALM_NULL != piReadLen)
        *piReadLen = 0;

    /************************************************************
    * 1. 打开文件
    ************************************************************/
    pLk = fopen(pcFile, "rb");
    if (ALM_NULL == pLk)
    {
        STF_printLog("file %s is not exist.", pcFile);
        return RET_ERR;
    }


    /************************************************************
    * 2. 获取文件长度并分配内存
    ************************************************************/

    /* 获取文件长度 */
    fseek(pLk, 0L, SEEK_END);
    iLkLen = ftell(pLk);

    if (0 == iLkLen)
    {
        fclose(pLk);
        return RET_ERR;
    }
    fseek(pLk, 0L, SEEK_SET);

    /* 分配内存 */
    *ppcBuf = (char*)malloc(iLkLen + 1);
    if (ALM_NULL == *ppcBuf)
    {
        STF_printLog("Malloc fail, malloc size is %d.", iLkLen + 1);
        fclose(pLk);
        Util_Free(*ppcBuf);
        *ppcBuf = ALM_NULL;

        return RET_ERR;
    }
    memset(*ppcBuf, 0, iLkLen + 1);

    /************************************************************
    * 3. 读取文件内容
    ************************************************************/
    iLen = fread(*ppcBuf, sizeof(char), iLkLen, pLk);
    if (iLen != iLkLen)
    {
        STF_printLog("Need read %d bytes, but only read %d bytes.",
            iLkLen,
            iLen);
        fclose(pLk);
        Util_Free(*ppcBuf);
        *ppcBuf = ALM_NULL;

        return RET_ERR;
    }
    if (ALM_NULL != piReadLen)
        *piReadLen = iLen;

    /************************************************************
    * 4. 关闭文件
    ************************************************************/
    fclose(pLk);

    return RET_OK;
}

//内存释放函数，仅在参数不为ALM_NULL时释放。并置为ALM_NULL。
void Test_FreePP(void** ppMem)
{
    if (ppMem != ALM_NULL)
    {
        if (ALM_NULL != *ppMem)
        {
            free(*ppMem);
        }

        *ppMem = ALM_NULL;
    }
}


#ifdef __cplusplus
#if __cplusplus
}
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */
