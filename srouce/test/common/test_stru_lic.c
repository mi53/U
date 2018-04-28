/*******************************************************************************
Description    : 以结构体ALM_LIC_STORE_STRU定义License文件。
History        :
<version> <date>     <Author> <Description>
*******************************************************************************/
#include "test_stru_lic.h"
#include "alm_tool_base.h"
#include "Tst_Framework.h"
#include "test_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */

ALM_INT32 ALM_LicStoreUpdateSbomSameNameID(INOUT ALM_LIC_STORE_SBOM_STRU* pstSbom,
        CONST ALM_LIC_STORE_S2BBOM_STRU* pstS2Bbom)
{
    ALM_INT32 iRet = ALM_OK;
    ALM_UINT32 uiSameNameId = 0;
    ALM_UINT32 uiIdx;
    ALM_INT32 iCmp;

    ALM_ChkRet_M(pstS2Bbom != ALM_NULL && pstSbom != ALM_NULL, ALM_ERR_NULL_PTR,
                 (ALM_NULL));

    /* 更新SBOM下同名BBOM编号 */
    for (uiIdx = 0; uiIdx < pstSbom->stS2Bboms.uiNum; ++uiIdx)
    {
        iCmp = strcmp(pstSbom->stS2Bboms.pstArray[uiIdx].stBbom.acBBomName,
                      pstS2Bbom->stBbom.acBBomName);

        if (0 == iCmp)
        {
            ++uiSameNameId;
        }
    }

    pstSbom->stS2Bboms.pstArray[pstSbom->stS2Bboms.uiNum].stBbom.uiSameNameId = uiSameNameId;

    return iRet;
}


//ALM_INT32 iEsn; /* 0:ANY、1:单个、2:多个 */
//ALM_INT32 iType; /* 0:DEMO 1COMM 2STICK 3EMERGENCY */
//ALM_INT32 iAddtion; /*0: 无， 1: 存在 */
//ALM_INT32 iGraceDays; /*0, 非0 */
/* 得到测试用License */
ALM_INT32 Test_LicGet(OUT ALM_LIC_STORE_STRU* pstLic, CONST TestLicDimSt* pstLicDim)
{
    ALM_INT32 iRet = ALM_OK;
    ALM_LIC_ESN_STRU stTempEsn;

    ALM_ChkRet_M(pstLic != ALM_NULL && pstLicDim != ALM_NULL,
        ALM_ERR_NULL_PTR, (ALM_NULL));

    (ALM_VOID)ALM_memset_s(pstLic, sizeof(ALM_LIC_STORE_STRU), 0,
        sizeof(ALM_LIC_STORE_STRU));

    pstLic->stGeneralInfo.uiAllowCount = 0;
    pstLic->stGeneralInfo.uiGraceDays = (ALM_UINT32)pstLicDim->iGraceDays;

    if (0 == pstLicDim->iType) {
        pstLic->stGeneralInfo.eLicType = ALM_LIC_TYPE_DEMO;
    } else if (1 == pstLicDim->iType) {
        pstLic->stGeneralInfo.eLicType = ALM_LIC_TYPE_COMM;
    } else {
        pstLic->stGeneralInfo.eLicType =
            (ALM_LIC_TYPE_ENUM)(ALM_LIC_TYPE_EMERGENCY + 1);//无效值
    }

    if (ALM_NULL == pstLicDim->pcLsn) {
        (ALM_VOID)ALM_strcpy_s(pstLic->stGeneralInfo.acLsn,
            sizeof(pstLic->stGeneralInfo.acLsn),
            TEST_LIC_LSN);
    } else {
        (ALM_VOID)ALM_strcpy_s(pstLic->stGeneralInfo.acLsn,
            sizeof(pstLic->stGeneralInfo.acLsn),
            pstLicDim->pcLsn);
    }

    (ALM_VOID)ALM_strcpy_s(pstLic->stGeneralInfo.acFilePrdName,
        sizeof(pstLic->stGeneralInfo.acFilePrdName),
        TEST_LIC_PRD_NAME);

    (ALM_VOID)ALM_strcpy_s(pstLic->stGeneralInfo.acFilePrdVer,
        sizeof(pstLic->stGeneralInfo.acFilePrdVer),
        TEST_LIC_PRD_VER);

    (ALM_VOID)ALM_strcpy_s(pstLic->stGeneralInfo.acSoftId,
        sizeof(pstLic->stGeneralInfo.acSoftId),
        TEST_LIC_SOFT_ID);

    if (0 == pstLicDim->iEsn)
    {
        strcpy(stTempEsn.acEsn, TEST_LIC_ESN_ANY);
        ALM_ArrayObjAddLab_M(pstLic->stGeneralInfo.stEsns,
            ALM_LIC_ESN_STRU, &stTempEsn, 1, LAB_END);
    }
    else if (1 == pstLicDim->iEsn)
    {
        strcpy(stTempEsn.acEsn, TEST_LIC_ESN_SINGLE);
        ALM_ArrayObjAddLab_M(pstLic->stGeneralInfo.stEsns,
            ALM_LIC_ESN_STRU, &stTempEsn, 1, LAB_END);
    }
    else if (2 == pstLicDim->iEsn)
    {
        ALM_INT32 idx;
        char acEsnTmp[ALM_CONST_ENV_ESN_MAX_LEN + 1];

		//ALM_LicStoreGeneralInit(&pstLic->stGeneralInfo,2);
        for (idx = 0; idx < TEST_LIC_ESN_MUL_NUM; ++idx)
        {
            strcpy(stTempEsn.acEsn, TEST_LIC_ESN_MUL(acEsnTmp, idx));
            ALM_ArrayObjAddLab_M(pstLic->stGeneralInfo.stEsns,
                ALM_LIC_ESN_STRU, &stTempEsn, 1, LAB_END);
        }
    }

        (ALM_VOID)ALM_strcpy_s(pstLic->stAddtion.acFileCopyRight,
            sizeof(pstLic->stAddtion.acFileCopyRight),
            "acFileCopyRight");

        (ALM_VOID)ALM_strcpy_s(pstLic->stAddtion.acFileCountry,
            sizeof(pstLic->stAddtion.acFileCountry),
            "acFileCountry");

        (ALM_VOID)ALM_strcpy_s(pstLic->stAddtion.acFileCreateTime,
            sizeof(pstLic->stAddtion.acFileCreateTime),
            TEST_LIC_CREATE_TIME);

        (ALM_VOID)ALM_strcpy_s(pstLic->stAddtion.acFileCreator,
            sizeof(pstLic->stAddtion.acFileCreator),
            "acFileCreator");

        (ALM_VOID)ALM_strcpy_s(pstLic->stAddtion.acFileCustom,
            sizeof(pstLic->stAddtion.acFileCustom),
            "acFileCustom");

        (ALM_VOID)ALM_strcpy_s(pstLic->stAddtion.acFileFormatVersion,
            sizeof(pstLic->stAddtion.acFileFormatVersion),
            "acFileFormatVersion");

        (ALM_VOID)ALM_strcpy_s(pstLic->stAddtion.acFileIssuer,
            sizeof(pstLic->stAddtion.acFileIssuer),
            "acFileIssuer");

        (ALM_VOID)ALM_strcpy_s(pstLic->stAddtion.acFileNodeDes,
            sizeof(pstLic->stAddtion.acFileNodeDes),
            "acFileNodeDes");

        (ALM_VOID)ALM_strcpy_s(pstLic->stAddtion.acFileNodeName,
            sizeof(pstLic->stAddtion.acFileNodeName),
            "acFileNodeName");

        (ALM_VOID)ALM_strcpy_s(pstLic->stAddtion.acFileOffice,
            sizeof(pstLic->stAddtion.acFileOffice),
            "acFileOffice");

        (ALM_VOID)ALM_strcpy_s(pstLic->stAddtion.acSwUpgradeDueDate,
            sizeof(pstLic->stAddtion.acSwUpgradeDueDate),
            "acSwUpgradeDueDate");

        pstLic->stAddtion.eLkLockModel = ALM_LIC_LK_LOCKMODEL_BUTT;
        pstLic->stAddtion.uiLkNodeSequense = 0;


    iRet = Test_LicGetBboms(&pstLic->stBbomInfo, pstLic->stGeneralInfo.acLsn,
        pstLicDim);
    ALM_CallLab_M(LAB_END, (ALM_NULL));
    
LAB_END:
    return iRet;
}


//    ALM_INT32 iDeadLine; /* 0永久、1临时、2永久+临时。*/
//    ALM_INT32 iRelDays; /* 0, 非0 */
//    ALM_INT32 iGraceDays; /*0, 非0 */
//    ALM_INT32 iResNum; /* 0无、1:1个、2:多个 */
//    ALM_INT32 iFuncNum; /* 0无、1:1个、2:多个 */
//    ALM_INT32 iGroup; /* 组：0无、1一个、2多个 */
//    ALM_INT32 iBbomSameName; /* 同名: 0无，1：必须同名, 2：允许同名  */
ALM_INT32 Test_LicGetBboms(INOUT ALM_LIC_STORE_BBOMS_STRU* pstBboms,
    CONST ALM_CHAR* pcLsn,
    CONST TestLicDimSt* pstLicDim)
{
    ALM_INT32 iRet = ALM_OK;
    ALM_LIC_STORE_BBOM_STRU stTmp;
    ALM_INT32 idx;
    ALM_INT32 idx2;
    ALM_INT32 iCount;
    ALM_BOOL bItrFun;
    ALM_CHAR acNameBuf[1024];

    ALM_ChkRet_M(pstBboms != ALM_NULL && pstLicDim != ALM_NULL,
        ALM_ERR_NULL_PTR, (ALM_NULL));

    for (idx = 0, iCount = 0, bItrFun = ALM_FALSE; ALM_TRUE; ++idx)
    {
        if (!bItrFun)
        {
            /* 迭代功能项 */
            if (iCount == pstLicDim->iResNum || idx == TEST_LIC_BBOM_MUL_NUM)
            {
                bItrFun = ALM_TRUE;
                iCount = 0;
                idx = 0;
                continue;
            }

            strcpy(stTmp.acBBomName, TEST_LIC_BBOM_RES(acNameBuf, idx));
            stTmp.ulValue = TEST_LIC_BBOM_RES_VALUE;
            stTmp.eType = ALM_LIC_BBOM_RESOURCE;
        }
        else
        {
            /* 迭代完毕 */
            if (iCount == pstLicDim->iFuncNum || idx == TEST_LIC_BBOM_MUL_NUM)
            {
                break;
            }

            strcpy(stTmp.acBBomName, TEST_LIC_BBOM_FUN(acNameBuf, idx));
            stTmp.ulValue = 1;
            stTmp.eType = ALM_LIC_BBOM_FUNCTION;
        }

        stTmp.stTimeInfo.uiGraceDays = (ALM_UINT32)pstLicDim->iGraceDays;
        stTmp.stTimeInfo.stValid.uiRelDays = (ALM_UINT32)pstLicDim->iRelDays;

        
        /* 填充deadline */
        if ((((idx >> 0) & 3) % 3) == 0) /* 永久 */
        {
            if (1 == pstLicDim->iDeadLine) {//不允许永久
                continue;
            }
            strcpy(stTmp.stTimeInfo.stValid.acDeadLine, TEST_LIC_DEAD_PERM);
        }
        else if ((((idx >> 0) & 3) % 3) == 1) /* 临时 */
        {
            if (0 == pstLicDim->iDeadLine) {//不允许临时
                continue;
            }
            strcpy(stTmp.stTimeInfo.stValid.acDeadLine, TEST_LIC_DEAD_TEMP);
        }
        else /* 永久 + 临时 */
        {
            if (2 != pstLicDim->iDeadLine) {//不能限制为单一截止日期类型
                continue;
            }

            //同名操作时处理永久+临时,此处填充永久
            strcpy(stTmp.stTimeInfo.stValid.acDeadLine, TEST_LIC_DEAD_PERM);
        }

        /* 填充group */
        if ((((idx >> 2) & 3) % 3) == 0) /* 空组 */
        {
            if (1 == pstLicDim->iGroup) {//不能为只有1个组
                continue;
            }
            stTmp.acGroupName[0] = '\0';
        }
        else if ((((idx >> 2) & 3) % 3) == 1) /* group1 */
        {
            if (0 == pstLicDim->iGroup) {//不能为只允许空组
                continue;
            }
            strcpy(stTmp.acGroupName, TEST_LIC_GROUP1);
        }
        else /* group2 */
        {
            if (2 != pstLicDim->iGroup) {//必须允许多个组
                continue;
            }
            strcpy(stTmp.acGroupName, TEST_LIC_GROUP1);
        }

        /* 处理same name 并添加BBOM */
        if ((((idx >> 4) & 3) % 2) == 0)/* 不同名，若为永久+临时，添加一个永久一个临时 */
        {
            iRet = ALM_LicStoreBbomsAdd(pstBboms, &stTmp);
            ALM_CallRet_M(iRet, (ALM_NULL));

            if (2 == pstLicDim->iDeadLine)
            {
                strcpy(stTmp.stTimeInfo.stValid.acDeadLine, TEST_LIC_DEAD_TEMP);

                iRet = ALM_LicStoreBbomsAdd(pstBboms, &stTmp);
                ALM_CallRet_M(iRet, (ALM_NULL));
            }
        }
        else /* 同名，若为永久+临时，添加2个永久，多个临时 */
        {
            iRet = ALM_LicStoreBbomsAdd(pstBboms, &stTmp);
            ALM_CallRet_M(iRet, (ALM_NULL));

            iRet = ALM_LicStoreBbomsAdd(pstBboms, &stTmp);
            ALM_CallRet_M(iRet, (ALM_NULL));

            if (2 == pstLicDim->iDeadLine)
            {
                strcpy(stTmp.stTimeInfo.stValid.acDeadLine, TEST_LIC_DEAD_TEMP);
                for (idx2 = 2; idx2 < TEST_LIC_BBOM_SAME_NAME_NUM; ++idx2)
                {
                    iRet = ALM_LicStoreBbomsAdd(pstBboms, &stTmp);
                    ALM_CallRet_M(iRet, (ALM_NULL));
                }
            }
        }

        ++iCount;
    }

    return iRet;
}

//    ALM_INT32 iSbomNum; /* 0无、1:1个、2:多个 */
//    ALM_INT32 iS2BbomNum; /* 0无、1:1个、2:多个 */
ALM_INT32 Test_LicGetSboms(INOUT ALM_LIC_STORE_SBOMS_STRU* pstSboms,
    CONST ALM_CHAR* pcLsn,
    CONST TestLicDimSt* pstLicDim)
{
    ALM_INT32 iRet = ALM_OK;
    ALM_LIC_STORE_SBOM_STRU stTmpSbom = {0};
    ALM_LIC_STORE_BBOMS_STRU stTmpBboms = {0};
    TestLicDimSt stDimToS2B;
    ALM_LIC_STORE_S2BBOM_STRU stS2BTmp;
    ALM_INT32 idx1;
    ALM_INT32 idx2;
    ALM_CHAR acNameBuf[1024];

    ALM_ChkRet_M(pstSboms != ALM_NULL && pstLicDim != ALM_NULL,
        ALM_ERR_NULL_PTR, (ALM_NULL));

    for (idx1 = 0; idx1 < TEST_LIC_SBOM_NUM; ++idx1)
    {
        strcpy(stTmpSbom.acChsDesc, "SBOM_CH_DESC");
        strcpy(stTmpSbom.acChsUnit, "SBOM_CH_UNIT");
        strcpy(stTmpSbom.acEngDesc, "SBOM_EN_DESC");
        strcpy(stTmpSbom.acEngUnit, "SBOM_EN_UNIT");
        strcpy(stTmpSbom.acName, TEST_LIC_SBOM_NAME(acNameBuf, idx1));
        stTmpSbom.stS2Bboms.uiCap = 0;
        stTmpSbom.stS2Bboms.uiNum = 0;
        stTmpSbom.stS2Bboms.pstArray = ALM_NULL;
        if (0 == pstLicDim->iSbomNum)
        {
            break;
        }
        else 
        {
            (ALM_VOID)ALM_memcpy_s(&stDimToS2B, sizeof(stDimToS2B),
                pstLicDim, sizeof(TestLicDimSt));
            for (idx2 = 0; idx2 < TEST_LIC_S2BOM_NUM; ++idx2)
            {
                if (0 == pstLicDim->iS2BbomNum)
                {
                    break;
                }
                else
                {
                    if (1 == pstLicDim->iS2BbomNum)
                    {
                        stDimToS2B.iResNum = idx2 % 2 == 0 ? 1 : 0;
                        stDimToS2B.iFuncNum = idx2 % 2 == 1 ? 1 : 0;
                        iRet = Test_LicGetBboms(&stTmpBboms, pcLsn, &stDimToS2B);
                        ALM_CallLab_M(LAB_END, (ALM_NULL));
                    }
                    else
                    {
                        iRet = Test_LicGetBboms(&stTmpBboms, pcLsn, &stDimToS2B);
                        ALM_CallLab_M(LAB_END, (ALM_NULL));
                    }
                }
            }

            for (idx2 = 0; idx2 < (ALM_INT32)stTmpBboms.uiNum; ++idx2)
            {
                (ALM_VOID)ALM_memcpy_s(&stS2BTmp.stBbom, sizeof(stS2BTmp.stBbom),
                    &stTmpBboms.pstArray[idx2],
                    sizeof(stTmpBboms.pstArray[idx2]));
                
                ALM_ArrayObjAddRet_M(stTmpSbom.stS2Bboms, ALM_LIC_STORE_S2BBOM_STRU, &stS2BTmp, 1);
            }

            ALM_ArrayFree_M(stTmpBboms.pstArray,
                ALM_LIC_STORE_BBOM_STRU,
                stTmpBboms.uiNum,
                &stTmpBboms.uiCap);

            iRet = ALM_LicStoreSbomsAdd(pstSboms, &stTmpSbom);
            ALM_ArrayObjCls0_M(stTmpSbom.stS2Bboms, ALM_LIC_STORE_S2BBOM_STRU);
            ALM_CallLab_M(LAB_END, (ALM_NULL));

            if (1 == pstLicDim->iSbomNum)
                break;
        }
    }

LAB_END:
    if (iRet != ALM_OK)
    {
        ALM_ArrayFree_M(stTmpBboms.pstArray,
            ALM_LIC_STORE_BBOM_STRU,
            stTmpBboms.uiNum,
            &stTmpBboms.uiCap);
        ALM_StClsLicStoreSboms(pstSboms);
    }
    return iRet;
}

/* 获得最小迭代的基准矢量 */
ALM_INT32 Test_LicMinItrHead(OUT ALM_LIC_STORE_STRU* pstLic, 
    OUT TestLicDimSt* pstLicDimItr)
{
    ALM_INT32 iRet = ALM_OK;
    TestLicDimSt stBase = TEST_LIC_ITR_HEAD;

    ALM_ChkRet_M(pstLic != ALM_NULL && pstLicDimItr != ALM_NULL,
        ALM_ERR_NULL_PTR, (ALM_NULL));

    (ALM_VOID)ALM_memcpy_s(pstLicDimItr, sizeof(TestLicDimSt), &stBase,
        sizeof(stBase));

    iRet = Test_LicGet(pstLic, pstLicDimItr);
    ALM_CallRet_M(iRet, (ALM_NULL));

    return iRet;
}

/* 最小迭代迭代下一项目，（每次仅有一个项目与基准矢量不一致） */
ALM_INT32 Test_LicMinItr(OUT ALM_LIC_STORE_STRU* pstLic, 
    INOUT TestLicDimSt* pstLicDimItr,
    OUT ALM_BOOL* pbEnd)
{
    ALM_INT32 iRet = ALM_OK;
    TestLicDimSt stBase = TEST_LIC_ITR_HEAD;
    ALM_BOOL bFindNotBase = ALM_FALSE;
    ALM_BOOL bNextToItr = ALM_FALSE;

    ALM_ChkRet_M(pstLic != ALM_NULL && pstLicDimItr != ALM_NULL,
        ALM_ERR_NULL_PTR, (ALM_NULL));

    (ALM_VOID)ALM_memset_s(pstLic, sizeof(ALM_LIC_STORE_STRU), 0,
        sizeof(ALM_LIC_STORE_STRU));

    if (0 == memcmp(pstLicDimItr, &stBase, sizeof(stBase)))
    {
        bNextToItr = ALM_TRUE;
    }

    /* 找到并迭代非基准项 */
#define _TEST_LIC_ITR_ITEM(item, field) \
    if (!bFindNotBase)\
    {\
        if (bNextToItr || (pstLicDimItr->item != stBase.item))\
        {\
            ++pstLicDimItr->item;\
            if (pstLicDimItr->item >= field)\
            {\
                pstLicDimItr->item = 0;\
            }\
            if (pstLicDimItr->item == stBase.item)\
            {\
                bNextToItr = ALM_TRUE;\
            }\
            else\
            {\
                bFindNotBase = ALM_TRUE;\
                bNextToItr = ALM_FALSE;\
            }\
        }\
    }    

    /* 更新定义 */
    _TEST_LIC_ITR_ITEM(iDeadLine, 3); /* 0永久、1临时、2永久+临时。*/
    _TEST_LIC_ITR_ITEM(iRelDays, 2); /* 0, 非0 */
    _TEST_LIC_ITR_ITEM(iGraceDays, 2); /*0, 非0 */
    _TEST_LIC_ITR_ITEM(iResNum, 3); /* 0无、1:1个、2:多个 */
    _TEST_LIC_ITR_ITEM(iFuncNum, 3); /* 0无、1:1个、2:多个 */
    _TEST_LIC_ITR_ITEM(iSbomNum, 3); /* 0无、1:1个、2:多个 */
    _TEST_LIC_ITR_ITEM(iS2BbomNum, 3); /* 0无、1:1个、2:多个 */
    _TEST_LIC_ITR_ITEM(iEsn, 3); /* 0:ANY、1:单个、2:多个 */
    _TEST_LIC_ITR_ITEM(iType, 2); /* 0:DEMO 1COMM  */
    _TEST_LIC_ITR_ITEM(iGroup, 3); /* 组：0无、1一个、2多个 */
    _TEST_LIC_ITR_ITEM(iBbomSameName, 3); /* 同名: 0无，1：多个, 2：0个及多个  */
    
    /*
    STF_printLog("%02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d\n",
        pstLicDimItr->iDeadLine,
        pstLicDimItr->iRelDays,
        pstLicDimItr->iGraceDays,
        pstLicDimItr->iResNum,
        pstLicDimItr->iFuncNum,
        pstLicDimItr->iSbomNum,
        pstLicDimItr->iS2BbomNum,
        pstLicDimItr->iEsn,
        pstLicDimItr->iType,
        pstLicDimItr->iAddtion,
        pstLicDimItr->iGroup,
        pstLicDimItr->iBbomSameName);
        */

    if (bNextToItr)
    {
        *pbEnd = ALM_TRUE;
    }
    else
    {
        *pbEnd = ALM_FALSE;

        /* 不生成无任何BBOM的License */
        if (0 == pstLicDimItr->iResNum && 0 == pstLicDimItr->iFuncNum)
        {
            iRet = Test_LicMinItr(pstLic, pstLicDimItr, pbEnd);
            ALM_CallRet_M(iRet, (ALM_NULL));
        }
        else
        {
            iRet = Test_LicGet(pstLic, pstLicDimItr);
            ALM_CallRet_M(iRet, (ALM_NULL));
        }
    }

#undef _TEST_LIC_ITR_ITEM
    return iRet;
}

/* 全量迭代 */
ALM_INT32 Test_LicItr(OUT ALM_LIC_STORE_STRU* pstLic, 
    INOUT TestLicDimSt* pstLicDimItr,
    OUT ALM_BOOL* pbEnd)
{
	
    ALM_INT32 iRet = ALM_OK;
    ALM_BOOL bContinue = ALM_TRUE; //是否进位

    ALM_ChkRet_M(pstLic != ALM_NULL && pstLicDimItr != ALM_NULL,
        ALM_ERR_NULL_PTR, (ALM_NULL));

    (ALM_VOID)ALM_memset_s(pstLic, sizeof(ALM_LIC_STORE_STRU), 0,
        sizeof(ALM_LIC_STORE_STRU));

#define _TEST_LIC_ITR_ITEM(item, field) \
    if (bContinue)\
    {\
        ++pstLicDimItr->item;\
        if (pstLicDimItr->item >= field) \
        {\
            pstLicDimItr->item = 0; \
        }\
        else \
        {\
            bContinue = ALM_FALSE;\
        }\
    }

    /* 更新定义 */
    _TEST_LIC_ITR_ITEM(iDeadLine, 3); /* 0永久、1临时、2永久+临时。*/
    _TEST_LIC_ITR_ITEM(iRelDays, 2); /* 0, 非0 */
    _TEST_LIC_ITR_ITEM(iGraceDays, 2); /*0, 非0 */
    _TEST_LIC_ITR_ITEM(iResNum, 3); /* 0无、1:1个、2:多个 */
    _TEST_LIC_ITR_ITEM(iFuncNum, 3); /* 0无、1:1个、2:多个 */
    _TEST_LIC_ITR_ITEM(iSbomNum, 3); /* 0无、1:1个、2:多个 */
    _TEST_LIC_ITR_ITEM(iS2BbomNum, 3); /* 0无、1:1个、2:多个 */
    _TEST_LIC_ITR_ITEM(iEsn, 3); /* 0:ANY、1:单个、2:多个 */
    _TEST_LIC_ITR_ITEM(iType, 2); /* 0:DEMO 1COMM  */
    _TEST_LIC_ITR_ITEM(iGroup, 3); /* 组：0无、1一个、2多个 */
    _TEST_LIC_ITR_ITEM(iBbomSameName, 3); /* 同名: 0无，1：多个, 2：0个及多个  */

    if (bContinue)
    {
        *pbEnd = ALM_TRUE;
    }
    else
    {
        *pbEnd = ALM_FALSE;

        /* 不生成无任何BBOM的License */
        if (0 == pstLicDimItr->iResNum && 0 == pstLicDimItr->iFuncNum)
        {
            iRet = Test_LicItr(pstLic, pstLicDimItr, pbEnd);
            ALM_CallRet_M(iRet, (ALM_NULL));
        }
        else
        {
            iRet = Test_LicGet(pstLic, pstLicDimItr);
            ALM_CallRet_M(iRet, (ALM_NULL));
        }
    }

#undef _TEST_LIC_ITR_ITEM
    return iRet;
}

/* 校验License文件定义与License文件内容是否匹配 */
ALM_INT32 Test_LicCmpByStStore(CONST ALM_LIC_STORE_STRU* pstLic,
    CONST ALM_LIC_STORE_STRU* pstLicByDim,
    OUT ALM_BOOL* pbRslt)
{
    ALM_INT32 iRet = ALM_OK;
    ALM_UINT32 uiIdx1;
    ALM_UINT32 uiIdx2;
    ALM_BOOL bTmp;

    ALM_ChkRet_M(pstLic != ALM_NULL && pstLicByDim != ALM_NULL
        && pbRslt != ALM_NULL, ALM_ERR_NULL_PTR, (ALM_NULL));
    *pbRslt = ALM_TRUE;

    /* 比较License文件附加内容 */

    TEST_LIC_CMP_STR(pstLic->stAddtion.acFileCopyRight,
        pstLicByDim->stAddtion.acFileCopyRight);
    TEST_LIC_CMP_STR(pstLic->stAddtion.acFileFormatVersion,
        pstLicByDim->stAddtion.acFileFormatVersion);
    TEST_LIC_CMP_STR(pstLic->stAddtion.acFileCreator,
        pstLicByDim->stAddtion.acFileCreator);
    TEST_LIC_CMP_STR(pstLic->stAddtion.acFileIssuer,
        pstLicByDim->stAddtion.acFileIssuer);
    TEST_LIC_CMP_STR(pstLic->stAddtion.acFileCreateTime,
        pstLicByDim->stAddtion.acFileCreateTime);
    TEST_LIC_CMP_STR(pstLic->stAddtion.acSwUpgradeDueDate,
        pstLicByDim->stAddtion.acSwUpgradeDueDate);

    TEST_LIC_CMP_STR(pstLic->stAddtion.acFileCountry,
        pstLicByDim->stAddtion.acFileCountry);
    TEST_LIC_CMP_STR(pstLic->stAddtion.acFileOffice,
        pstLicByDim->stAddtion.acFileOffice);
    TEST_LIC_CMP_STR(pstLic->stAddtion.acFileCustom,
        pstLicByDim->stAddtion.acFileCustom);

    TEST_LIC_CMP_VAL(pstLic->stAddtion.uiLkNodeSequense,
        pstLicByDim->stAddtion.uiLkNodeSequense);
    TEST_LIC_CMP_VAL(pstLic->stAddtion.eLkLockModel,
        pstLicByDim->stAddtion.eLkLockModel);
    TEST_LIC_CMP_STR(pstLic->stAddtion.acFileNodeName,
        pstLicByDim->stAddtion.acFileNodeName);
    TEST_LIC_CMP_STR(pstLic->stAddtion.acFileNodeDes,
        pstLicByDim->stAddtion.acFileNodeDes);

    /* 比较License文件通用内容 */
    TEST_LIC_CMP_STR(pstLic->stGeneralInfo.acLsn,
        pstLicByDim->stGeneralInfo.acLsn);
    TEST_LIC_CMP_VAL(pstLic->stGeneralInfo.eLicType,
        pstLicByDim->stGeneralInfo.eLicType);
    TEST_LIC_CMP_STR(pstLic->stGeneralInfo.acFilePrdName,
        pstLicByDim->stGeneralInfo.acFilePrdName);
    TEST_LIC_CMP_STR(pstLic->stGeneralInfo.acFilePrdVer,
        pstLicByDim->stGeneralInfo.acFilePrdVer);
    TEST_LIC_CMP_VAL(pstLic->stGeneralInfo.uiAllowCount,
        pstLicByDim->stGeneralInfo.uiAllowCount);
    TEST_LIC_CMP_VAL(pstLic->stGeneralInfo.uiGraceDays,
        pstLicByDim->stGeneralInfo.uiGraceDays);
    TEST_LIC_CMP_STR(pstLic->stGeneralInfo.acSoftId,
        pstLicByDim->stGeneralInfo.acSoftId);

    /* 比较esn */
    TEST_LIC_CMP_VAL(pstLic->stGeneralInfo.stEsns.uiNum,
        pstLicByDim->stGeneralInfo.stEsns.uiNum);
    for (uiIdx1 = 0; uiIdx1 < pstLic->stGeneralInfo.stEsns.uiNum; ++uiIdx1)
    {
        bTmp = ALM_FALSE;
        for (uiIdx2 = 0; uiIdx2 < pstLicByDim->stGeneralInfo.stEsns.uiNum;
            ++uiIdx2)
        {
            if (0 == strcmp(pstLic->stGeneralInfo.stEsns.pstArray[uiIdx1].acEsn,
                pstLicByDim->stGeneralInfo.stEsns.pstArray[uiIdx2].acEsn))
            {
                bTmp = ALM_TRUE;
                break;
            }
        }

        if (!bTmp) {
            *pbRslt = ALM_FALSE;
            goto LAB_END;
        }
    }

    /* 比较License文件BBOM内容 */
    iRet = _TestLicCmpBbom(&pstLic->stBbomInfo, &pstLicByDim->stBbomInfo,
        pbRslt);
    ALM_CallLab_M(LAB_END, (ALM_NULL));

    iRet = _TestLicCmpOffer(&pstLic->stOfferInfo, &pstLicByDim->stOfferInfo,
        pbRslt);
    ALM_CallLab_M(LAB_END, ("cmp offer fail!"));

    if (!*pbRslt) {
        ALM_DbgShowTable("ALM_LIC_BBOM");
        goto LAB_END;
    }

    if (!*pbRslt) {
        goto LAB_END;
    }

LAB_END:
    return iRet;
}

/* 校验License文件定义与License文件内容是否匹配 */
ALM_INT32 Test_LicCheck(CONST ALM_LIC_STORE_STRU* pstLic,
    CONST TestLicDimSt* pstLicDim,
    OUT ALM_BOOL* pbRslt)
{
    ALM_INT32 iRet = ALM_OK;
    ALM_LIC_STORE_STRU stLicByDim;

    ALM_ChkRet_M(pstLic != ALM_NULL && pstLicDim != ALM_NULL
        && pbRslt != ALM_NULL, ALM_ERR_NULL_PTR, (ALM_NULL));

    iRet = Test_LicGet(&stLicByDim, pstLicDim);
    ALM_CallRet_M(iRet, (ALM_NULL));
    
    iRet = Test_LicCmpByStStore(pstLic, &stLicByDim, pbRslt);
    ALM_CallRet_M(iRet, (ALM_NULL));

    ALM_StClsLicStore(&stLicByDim);
    return iRet;
}

static ALM_INT32 _TestLicCmpS2Bbom(CONST ALM_LIC_STORE_S2BBOMS_STRU* pstLeft,
    CONST ALM_LIC_STORE_S2BBOMS_STRU* pstRight,
    OUT ALM_BOOL* pbRslt)
{
    ALM_INT32 iRet = ALM_OK;
    ALM_UINT32 i1, i2;
    ALM_BOOL bTmp = ALM_FALSE;

    *pbRslt = ALM_TRUE;

    TEST_LIC_CMP_VAL(pstLeft->uiNum, pstRight->uiNum);

    for (i1 = 0; i1 < pstLeft->uiNum; ++i1)
    {
        bTmp = ALM_FALSE;
        for (i2 = 0; i2 < pstRight->uiNum; ++i2)
        {
            if (0 == strcmp(pstLeft->pstArray[i1].stBbom.acBBomName,
                pstRight->pstArray[i2].stBbom.acBBomName)
                && pstLeft->pstArray[i1].stBbom.uiSameNameId
                == pstRight->pstArray[i2].stBbom.uiSameNameId)
            {
                bTmp = ALM_TRUE;

                TEST_LIC_CMP_STR(
                    pstLeft->pstArray[i1].stBbom.acGroupName,
                    pstRight->pstArray[i2].stBbom.acGroupName);
                TEST_LIC_CMP_VAL(
                    pstLeft->pstArray[i1].stBbom.ulValue,
                    pstRight->pstArray[i2].stBbom.ulValue);
                TEST_LIC_CMP_VAL(
                    pstLeft->pstArray[i1].stBbom.eType,
                    pstRight->pstArray[i2].stBbom.eType);
                TEST_LIC_CMP_STR(
                    pstLeft->pstArray[i1].stBbom.stTimeInfo.stValid.acDeadLine,
                    pstRight->pstArray[i2].stBbom.stTimeInfo.stValid.acDeadLine);
                TEST_LIC_CMP_VAL(
                    pstLeft->pstArray[i1].stBbom.stTimeInfo.stValid.uiRelDays,
                    pstRight->pstArray[i2].stBbom.stTimeInfo.stValid.uiRelDays);
                TEST_LIC_CMP_VAL(
                    pstLeft->pstArray[i1].stBbom.stTimeInfo.uiGraceDays,
                    pstRight->pstArray[i2].stBbom.stTimeInfo.uiGraceDays);
            }
        }

        if (!bTmp) {
            *pbRslt = ALM_FALSE;
            goto LAB_END;
        }
    }

LAB_END:

    if (bTmp && !*pbRslt)
    {
        STF_printLog("Check S2Bbom fail: %s\n",
            pstLeft->pstArray[i1].stBbom.acBBomName);
    }

    return iRet;
}


static ALM_INT32 _TestLicCmpBbom(CONST ALM_LIC_STORE_BBOMS_STRU* pstLeft,
    CONST ALM_LIC_STORE_BBOMS_STRU* pstRight,
    OUT ALM_BOOL* pbRslt)
{
    ALM_INT32 iRet = ALM_OK;
    ALM_UINT32 uiIdx1;
    ALM_UINT32 uiIdx2;
    ALM_BOOL bTmp = ALM_FALSE;

    *pbRslt = ALM_TRUE;

    TEST_LIC_CMP_VAL(pstLeft->uiNum, pstRight->uiNum);
    for (uiIdx1 = 0; uiIdx1 < pstLeft->uiNum; ++uiIdx1)
    {
        bTmp = ALM_FALSE;
        for (uiIdx2 = 0; uiIdx2 < pstRight->uiNum; ++uiIdx2)
        {
            if (0 == strcmp(pstLeft->pstArray[uiIdx1].acBBomName,
                    pstRight->pstArray[uiIdx2].acBBomName)
                && pstLeft->pstArray[uiIdx1].uiSameNameId
                == pstRight->pstArray[uiIdx2].uiSameNameId)
            {
                bTmp = ALM_TRUE;

                TEST_LIC_CMP_STR(pstLeft->pstArray[uiIdx1].acGroupName,
                    pstRight->pstArray[uiIdx2].acGroupName);
                TEST_LIC_CMP_VAL(pstLeft->pstArray[uiIdx1].ulValue,
                    pstRight->pstArray[uiIdx2].ulValue);
                TEST_LIC_CMP_VAL(pstLeft->pstArray[uiIdx1].eType,
                    pstRight->pstArray[uiIdx2].eType);
                TEST_LIC_CMP_STR(
                    pstLeft->pstArray[uiIdx1].stTimeInfo.stValid.acDeadLine,
                    pstRight->pstArray[uiIdx2].stTimeInfo.stValid.acDeadLine);
                TEST_LIC_CMP_VAL(
                    pstLeft->pstArray[uiIdx1].stTimeInfo.stValid.uiRelDays,
                    pstRight->pstArray[uiIdx2].stTimeInfo.stValid.uiRelDays);
                TEST_LIC_CMP_VAL(
                    pstLeft->pstArray[uiIdx1].stTimeInfo.uiGraceDays,
                    pstRight->pstArray[uiIdx2].stTimeInfo.uiGraceDays);

                break;
            }
        }

        if (!bTmp) {
            *pbRslt = ALM_FALSE;
            goto LAB_END;
        }
    }

LAB_END:
    if (bTmp && !*pbRslt)
    {
        STF_printLog("Check Bbom fail: %s\n",
            pstLeft->pstArray[uiIdx1].acBBomName);
    }
    return iRet;
}

static ALM_INT32 _TestLicCmpOffer(CONST ALM_LIC_STORE_OFFERS_STRU* pstLeft,
                                   CONST ALM_LIC_STORE_OFFERS_STRU* pstRight,
                                   OUT ALM_BOOL* pbRslt)
{
    ALM_INT32 iRet = ALM_OK;
    ALM_UINT32 uiIdx1;
    ALM_UINT32 uiIdx2;
    ALM_BOOL bTmp = ALM_FALSE;

    *pbRslt = ALM_TRUE;

    TEST_LIC_CMP_VAL(pstLeft->uiNum, pstRight->uiNum);

    for (uiIdx1 = 0; uiIdx1 < pstLeft->uiNum; ++uiIdx1)
    {
        bTmp = ALM_FALSE;

        for (uiIdx2 = 0; uiIdx2 < pstRight->uiNum; ++uiIdx2)
        {
            if (0 == strcmp(pstLeft->pstArray[uiIdx1].acOfferName,
                            pstRight->pstArray[uiIdx2].acOfferName)
                && 0 == strcmp(pstLeft->pstArray[uiIdx1].acOfferVer,
                               pstRight->pstArray[uiIdx2].acOfferVer))
            {
                bTmp = ALM_TRUE;
                //比较sboms
                iRet = _TestLicCmpSbom(&pstLeft->pstArray[uiIdx1].stSboms,
                                        &pstRight->pstArray[uiIdx2].stSboms, pbRslt);
                ALM_CallRet_M(iRet, ("Offer:%s %s 's sboms fail", pstLeft->pstArray[uiIdx1].acOfferName, pstLeft->pstArray[uiIdx1].acOfferVer));
                //比较PackageItems
                iRet = _TestLicCmpPackageItem(&pstLeft->pstArray[uiIdx1].stPackageItems,
                                               &pstRight->pstArray[uiIdx2].stPackageItems, pbRslt);
                ALM_CallRet_M(iRet, ("Offer:%s %s 's packageitems fail", pstLeft->pstArray[uiIdx1].acOfferName, pstLeft->pstArray[uiIdx1].acOfferVer));
                break;
            }
        }

        if (!bTmp)
        {
            *pbRslt = ALM_FALSE;
            goto LAB_END;
        }
    }

LAB_END:

    if (bTmp && !*pbRslt)
    {
        STF_printLog("Check Offer fail: %s %s\n",
                     pstLeft->pstArray[uiIdx1].acOfferName, pstLeft->pstArray[uiIdx1].acOfferVer);
    }

    return iRet;
}

static ALM_INT32 _TestLicCmpSbom(CONST ALM_LIC_STORE_SBOMS_STRU* pstLeft,
                                  CONST ALM_LIC_STORE_SBOMS_STRU* pstRight,
                                  OUT ALM_BOOL* pbRslt)
{
    ALM_INT32 iRet = ALM_OK;
    ALM_UINT32 uiIdx1;
    ALM_UINT32 uiIdx2;
    ALM_BOOL bTmp = ALM_FALSE;

    *pbRslt = ALM_TRUE;

    TEST_LIC_CMP_VAL(pstLeft->uiNum, pstRight->uiNum);

    for (uiIdx1 = 0; uiIdx1 < pstLeft->uiNum; ++uiIdx1)
    {
        bTmp = ALM_FALSE;

        for (uiIdx2 = 0; uiIdx2 < pstRight->uiNum; ++uiIdx2)
        {
            if (0 == strcmp(pstLeft->pstArray[uiIdx1].acName,
                            pstRight->pstArray[uiIdx2].acName))
            {
                bTmp = ALM_TRUE;

                TEST_LIC_CMP_STR(pstLeft->pstArray[uiIdx1].acChsDesc,
                                 pstRight->pstArray[uiIdx2].acChsDesc);
                TEST_LIC_CMP_STR(pstLeft->pstArray[uiIdx1].acChsUnit,
                                 pstRight->pstArray[uiIdx2].acChsUnit);
                TEST_LIC_CMP_STR(pstLeft->pstArray[uiIdx1].acEngDesc,
                                 pstRight->pstArray[uiIdx2].acEngDesc);
                TEST_LIC_CMP_STR(pstLeft->pstArray[uiIdx1].acEngUnit,
                                 pstRight->pstArray[uiIdx2].acEngUnit);
                TEST_LIC_CMP_VAL(pstLeft->pstArray[uiIdx1].eSaleItemType,
                                 pstRight->pstArray[uiIdx2].eSaleItemType);
                //比较value
                iRet = _TestLicCmpSbomValue(&pstLeft->pstArray[uiIdx1].stValues,
                                            &pstRight->pstArray[uiIdx2].stValues, pbRslt);
                ALM_CallRet_M(iRet, ("Sbom:%s 's Value fail", pstLeft->pstArray[uiIdx1].acName));
                //比较sboms
                iRet = _TestLicCmpS2Bbom(&pstLeft->pstArray[uiIdx1].stS2Bboms,
                                         &pstRight->pstArray[uiIdx2].stS2Bboms, pbRslt);
                ALM_CallRet_M(iRet, ("Sbom:%s 's S2bom fail", pstLeft->pstArray[uiIdx1].acName));
                break;
            }
        }

        if (!bTmp)
        {
            *pbRslt = ALM_FALSE;
            goto LAB_END;
        }
    }

LAB_END:

    if (bTmp && !*pbRslt)
    {
        STF_printLog("Check sbom fail: %s\n",
                     pstLeft->pstArray[uiIdx1].acName);
    }

    return iRet;
}

static ALM_INT32 _TestLicCmpSbomValue(CONST ALM_LIC_STORE_SBOM_VALUES_STRU* pstLeft,
    CONST ALM_LIC_STORE_SBOM_VALUES_STRU* pstRight,
    OUT ALM_BOOL* pbRslt)
{
    ALM_INT32 iRet = ALM_OK;
    ALM_UINT32 uiIdx1;
    ALM_UINT32 uiIdx2;
    ALM_BOOL bTmp = ALM_FALSE;

    *pbRslt = ALM_TRUE;

    TEST_LIC_CMP_VAL(pstLeft->uiNum, pstRight->uiNum);
    for (uiIdx1 = 0; uiIdx1 < pstLeft->uiNum; ++uiIdx1)
    {
        bTmp = ALM_FALSE;
        for (uiIdx2 = 0; uiIdx2 < pstRight->uiNum; ++uiIdx2)
        {
            if (pstLeft->pstArray[uiIdx1].ulVal
                    ==pstRight->pstArray[uiIdx2].ulVal)
            {
                bTmp = ALM_TRUE;

                TEST_LIC_CMP_STR(pstLeft->pstArray[uiIdx1].acDeadline,
                    pstRight->pstArray[uiIdx2].acDeadline);
                TEST_LIC_CMP_VAL(pstLeft->pstArray[uiIdx1].bIsnoControl,
                    pstRight->pstArray[uiIdx2].bIsnoControl);

                break;
            }
        }

        if (!bTmp) {
            *pbRslt = ALM_FALSE;
            goto LAB_END;
        }
    }

LAB_END:
    if (bTmp && !*pbRslt)
    {
        STF_printLog("Check sbomvaue fail: %d\n",
            pstLeft->pstArray[uiIdx1].ulVal);
    }
    return iRet;
}

static ALM_INT32 _TestLicCmpPackageItem(CONST ALM_LIC_STORE_PACKAGEITEMS_STRU* pstLeft,
        CONST ALM_LIC_STORE_PACKAGEITEMS_STRU* pstRight,
        OUT ALM_BOOL* pbRslt)
{
    ALM_INT32 iRet = ALM_OK;
    ALM_UINT32 uiIdx1;
    ALM_UINT32 uiIdx2;
    ALM_BOOL bTmp = ALM_FALSE;

    *pbRslt = ALM_TRUE;

    TEST_LIC_CMP_VAL(pstLeft->uiNum, pstRight->uiNum);

    for (uiIdx1 = 0; uiIdx1 < pstLeft->uiNum; ++uiIdx1)
    {
        bTmp = ALM_FALSE;

        for (uiIdx2 = 0; uiIdx2 < pstRight->uiNum; ++uiIdx2)
        {
            if (0 == strcmp(pstLeft->pstArray[uiIdx1].acName,
                            pstRight->pstArray[uiIdx2].acName))
            {
                bTmp = ALM_TRUE;

                TEST_LIC_CMP_STR(pstLeft->pstArray[uiIdx1].acChsDesc,
                                 pstRight->pstArray[uiIdx2].acChsDesc);
                TEST_LIC_CMP_STR(pstLeft->pstArray[uiIdx1].acChsUnit,
                                 pstRight->pstArray[uiIdx2].acChsUnit);
                TEST_LIC_CMP_STR(pstLeft->pstArray[uiIdx1].acEngDesc,
                                 pstRight->pstArray[uiIdx2].acEngDesc);
                TEST_LIC_CMP_STR(pstLeft->pstArray[uiIdx1].acEngUnit,
                                 pstRight->pstArray[uiIdx2].acEngUnit);

                //比较value
                iRet = _TestLicCmpPackageItemValue(&pstLeft->pstArray[uiIdx1].stValues,
                                                   &pstRight->pstArray[uiIdx2].stValues, pbRslt);
                ALM_CallRet_M(iRet, ("Sbom:%s 's Value fail", pstLeft->pstArray[uiIdx1].acName));
                //比较p2s
                /*        iRet = _TestLicCmpS2Bbom(&pstLeft->pstArray[uiIdx1].stS2Bboms,
                                                 &pstRight->pstArray[uiIdx2].stS2Bboms, pbRslt);
                        ALM_CallRet_M(iRet, ("Sbom:%s 's S2bom fail", pstLeft->pstArray[uiIdx1].acName));*/
                break;
            }
        }

        if (!bTmp)
        {
            *pbRslt = ALM_FALSE;
            goto LAB_END;
        }
    }

LAB_END:

    if (bTmp && !*pbRslt)
    {
        STF_printLog("Check sbom fail: %s\n",
                     pstLeft->pstArray[uiIdx1].acName);
    }

    return iRet;
}

static ALM_INT32 _TestLicCmpPackageItemValue(CONST ALM_LIC_STORE_PACKAGEITEM_VALUES_STRU* pstLeft,
    CONST ALM_LIC_STORE_PACKAGEITEM_VALUES_STRU* pstRight,
    OUT ALM_BOOL* pbRslt)
{
    ALM_INT32 iRet = ALM_OK;
    ALM_UINT32 uiIdx1;
    ALM_UINT32 uiIdx2;
    ALM_BOOL bTmp = ALM_FALSE;

    *pbRslt = ALM_TRUE;

    TEST_LIC_CMP_VAL(pstLeft->uiNum, pstRight->uiNum);
    for (uiIdx1 = 0; uiIdx1 < pstLeft->uiNum; ++uiIdx1)
    {
        bTmp = ALM_FALSE;
        for (uiIdx2 = 0; uiIdx2 < pstRight->uiNum; ++uiIdx2)
        {
            if (pstLeft->pstArray[uiIdx1].ulVal
                    ==pstRight->pstArray[uiIdx2].ulVal)
            {
                bTmp = ALM_TRUE;

                TEST_LIC_CMP_STR(pstLeft->pstArray[uiIdx1].acDeadline,
                    pstRight->pstArray[uiIdx2].acDeadline);

                break;
            }
        }

        if (!bTmp) {
            *pbRslt = ALM_FALSE;
            goto LAB_END;
        }
    }

LAB_END:
    if (bTmp && !*pbRslt)
    {
        STF_printLog("Check PackageItem vaue fail: %d\n",
            pstLeft->pstArray[uiIdx1].ulVal);
    }
    return iRet;
}

/* 生成名字 */
static ALM_CHAR* _TEST_LicNameCat(ALM_CHAR* acBuf, CONST ALM_CHAR* pcPref, ALM_INT32 idx)
{
    sprintf(acBuf, "%s%d", pcPref, idx);

    return acBuf;
}

ALM_INT32 ALM_LicStoreQuery2(CONST ALM_CHAR* pcLsn,
    ALM_UINT32 uiLsnLen,
    OUT ALM_LIC_STORE_STRU* pstQuery,
    OUT ALM_BOOL* pbExist)
{
    ALM_INT32 iRet;
    ALM_LIC_STORE_STRU* pstGet;

    memset(pstQuery, 0, sizeof(ALM_LIC_STORE_STRU));
    *pbExist = ALM_FALSE;

    iRet = ALM_LicStoreQuery(pcLsn, &pstGet);
    ALM_CallRet_M(iRet, (ALM_NULL));

    if (pstGet != ALM_NULL)
    {
        *pbExist = ALM_TRUE;
        iRet = ALM_LicStoreCpy(pstGet, pstQuery);
        ALM_CallRet_M(iRet, (ALM_NULL));
    }


    return ALM_OK;
}

ALM_INT32 ALM_LicStoreSave2(CONST ALM_LIC_STORE_STRU* pstLic)
{
    ALM_INT32 iRet;
    ALM_LIC_STORE_STRU* pstCpy;
    ALM_BOOL bEqual;
    
    pstCpy = (ALM_LIC_STORE_STRU*)OS_MemAlmAlloc(sizeof(ALM_LIC_STORE_STRU));
    memset(pstCpy, 0, sizeof(ALM_LIC_STORE_STRU));

    iRet = ALM_LicStoreCpy(pstLic, pstCpy);
    ALM_CallLab_M(LAB_END, (ALM_NULL));

    iRet = Test_LicCmpByStStore(pstLic, pstCpy, &bEqual);
    ALM_CallLab_M(LAB_END, (ALM_NULL));
    ALM_ChkLab_M(!(!bEqual && (iRet = ALM_ERR_UNKNOW) != ALM_OK),
        LAB_END, (ALM_NULL));

    iRet = ALM_LicStoreSave(pstCpy, ALM_NULL);
    ALM_CallLab_M(LAB_END, (ALM_NULL));

LAB_END:
    if (iRet != ALM_OK)
    {
        ALM_StClsLicStore(pstCpy);
    }

    return iRet;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */
