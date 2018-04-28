/*******************************************************************************
Description    : 以结构体ALM_LIC_STORE_STRU定义License文件。
History        :
<version> <date>     <Author> <Description>
*******************************************************************************/
#ifndef __TEST_LIC_STRU_LIC_H__
#define __TEST_LIC_STRU_LIC_H__

#include "alm_lic_store.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */

    
/* 定义测试用的License文件 */
typedef struct {
    CONST ALM_CHAR* pcLsn; /* LSN，若为ALM_NULL使用默认值 */
    ALM_INT32 iDeadLine; /* 0永久、1临时、2永久+临时。*/
    ALM_INT32 iRelDays; /* 0, 非0 */
    ALM_INT32 iGraceDays; /*0, 非0 */
    ALM_INT32 iResNum; /* 0无、1:1个、2:多个 */
    ALM_INT32 iFuncNum; /* 0无、1:1个、2:多个 */
    ALM_INT32 iSbomNum; /* 0无、1:1个、2:多个 */
    ALM_INT32 iS2BbomNum; /* 0无、1:1个、2:多个 */
    ALM_INT32 iEsn; /* 0:ANY、1:单个、2:多个 */
    ALM_INT32 iType; /* 0:DEMO 1COMM  */
    ALM_INT32 iGroup; /* 组：0无、1一个、2多个 */
    ALM_INT32 iBbomSameName; /* 同名: 0无，1：多个, 2：0个及多个  */
}TestLicDimSt;

/* 获得最小迭代的基准矢量 */
ALM_INT32 Test_LicMinItrHead(OUT ALM_LIC_STORE_STRU* pstLic, 
    OUT TestLicDimSt* pstLicDimItr);

/* 最小迭代迭代下一项目，（每次仅有一个项目与基准矢量不一致） */
ALM_INT32 Test_LicMinItr(OUT ALM_LIC_STORE_STRU* pstLic, 
    INOUT TestLicDimSt* pstLicDimItr,
    OUT ALM_BOOL* pbEnd);

/* 默认LSN，永久+临时，无相对时间，宽限60，多个资源项及功能项，多个SBOM几S2B，多个ESN，COMM，存在附加，多组，存在同名 */
#define TEST_LIC_ITR_HEAD {ALM_NULL, 2, 0, 1, 2, 2, 2, 2, 2, 1,  2, 2}

/* 全量迭代 */
ALM_INT32 Test_LicItr(OUT ALM_LIC_STORE_STRU* pstLic, 
    INOUT TestLicDimSt* pstLicDimItr,
    OUT ALM_BOOL* pbEnd);

/* 得到测试用License */
ALM_INT32 Test_LicGet(OUT ALM_LIC_STORE_STRU* pstLic,
    CONST TestLicDimSt* pstLicDim);

/* 校验License文件定义与License文件内容是否匹配 */
ALM_INT32 Test_LicCheck(CONST ALM_LIC_STORE_STRU* pstLic,
    CONST TestLicDimSt* pstLicDim,
    OUT ALM_BOOL* pbRslt);

/* 校验License文件定义与License文件内容是否匹配 */
ALM_INT32 Test_LicCmpByStStore(CONST ALM_LIC_STORE_STRU* pstLic,
    CONST ALM_LIC_STORE_STRU* pstLicByDim,
    OUT ALM_BOOL* pbRslt);

//    ALM_INT32 iDeadLine; /* 0永久、1临时、2永久+临时。*/
//    ALM_INT32 iRelDays; /* 0, 非0 */
//    ALM_INT32 iGraceDays; /*0, 非0 */
//    ALM_INT32 iResNum; /* 0无、1:1个、2:多个 */
//    ALM_INT32 iFuncNum; /* 0无、1:1个、2:多个 */
//    ALM_INT32 iGroup; /* 组：0无、1一个、2多个 */
//    ALM_INT32 iBbomSameName; /* 同名: 0无，1：必须同名, 2：允许同名  */
ALM_INT32 Test_LicGetBboms(INOUT ALM_LIC_STORE_BBOMS_STRU* pstBboms,
    CONST ALM_CHAR* pcLsn,
    CONST TestLicDimSt* pstLicDim);

/* 定义License所用信息 */
#define TEST_LIC_BBOM_RES_NAME_BASE "BbomRes"
#define TEST_LIC_BBOM_RES(acBuf, i)\
    _TEST_LicNameCat(acBuf, TEST_LIC_BBOM_RES_NAME_BASE, i)
#define TEST_LIC_BBOM_RES_NAME(i) TEST_LIC_BBOM_RES_NAME_BASE#i
#define TEST_LIC_BBOM_RES_VALUE 100

/* 生成名字 */
static ALM_CHAR* _TEST_LicNameCat(ALM_CHAR* acBuf, CONST ALM_CHAR* pcPref, ALM_INT32 idx);
/* 
* 指定同名、存在同名BBOM的同名数目。
* 永久+临时，在没有指定同名时只存在1个临时、1个永久，若指定同名存在多个临时及2个永久。
*/
#define TEST_LIC_BBOM_SAME_NAME_NUM 5 

#define TEST_LIC_BBOM_FUN_NAME_BASE "BbomFun"
#define TEST_LIC_BBOM_FUN(acBuf, i) \
    _TEST_LicNameCat(acBuf, TEST_LIC_BBOM_FUN_NAME_BASE, i)
#define TEST_LIC_BBOM_FUN_NAME(i) TEST_LIC_BBOM_FUN_NAME_BASE#i

#define TEST_LIC_BBOM_MUL_NUM 100 /* 多个资源项或功能项时，资源项、功能项的最大不同名数目 */

#define TEST_LIC_GROUP1 "Group1"
#define TEST_LIC_GROUP2 "Group1"
#define TEST_LIC_GROUP3 "Group1"

#define TEST_LIC_SBOM_NAME(acBuf, i) _TEST_LicNameCat(acBuf, "Sbom", i)

#define TEST_LIC_SBOM_VALUE 100
#define TEST_LIC_SBOM_NUM 5
#define TEST_LIC_S2BOM_NUM 5

#define TEST_LIC_ESN_ANY "ANY"
#define TEST_LIC_ESN_SINGLE "esn0"
#define TEST_LIC_ESN_MUL(acBuf, i) _TEST_LicNameCat(acBuf, "esn", i)
#define TEST_LIC_ESN_MUL_NUM 100

#define TEST_LIC_DEAD_PERM "PERMANENT"
#define TEST_LIC_DEAD_TEMP "2000-01-01"
#define TEST_LIC_CREATE_TIME "1990-01-01"

#define TEST_LIC_PRD_NAME "ProductName"
#define TEST_LIC_PRD_VER "ProductVer"

#define TEST_LIC_LSN "LIC20000101000000"
#define TEST_LIC_SOFT_ID "460-102-1989"

ALM_INT32 ALM_LicStoreQuery2(CONST ALM_CHAR* pcLsn,
    ALM_UINT32 uiLsnLen,
    OUT ALM_LIC_STORE_STRU* pstQuery,
    OUT ALM_BOOL* pbExist);

ALM_INT32 ALM_LicStoreSave2(CONST ALM_LIC_STORE_STRU* pstLic);

/************************************************************************
* 内部符号及函数定义
************************************************************************/
#define TEST_LIC_CMP_STR(str1, str2) \
    do {\
        if (0 != strcmp(str1, str2))\
        {\
            *pbRslt = ALM_FALSE;\
            STF_printLog("[ERR]: compare \"%s\" fail, [ \"%s\", \"%s\"].\n",\
                #str1, str1, str2);\
            goto LAB_END;\
        }\
    } while (0)

#define TEST_LIC_CMP_VAL(val1, val2) \
    do {\
        if (val1 != val2)\
        {\
            *pbRslt = ALM_FALSE;\
            STF_printLog("[ERR]: compare \"%s\" fail, [ \"%d\", \"%d\"].\n",\
                #val1, val1, val2);\
            goto LAB_END;\
        }\
    } while (0)

/* 比较指定内容是否相同，相同得到ALM_TRUE */
static ALM_INT32 _TestLicCmpSbom(CONST ALM_LIC_STORE_SBOMS_STRU* pstLeft,
    CONST ALM_LIC_STORE_SBOMS_STRU* pstRight,
    OUT ALM_BOOL* pbRslt);

static ALM_INT32 _TestLicCmpBbom(CONST ALM_LIC_STORE_BBOMS_STRU* pstLeft,
    CONST ALM_LIC_STORE_BBOMS_STRU* pstRight,
    OUT ALM_BOOL* pbRslt);

static ALM_INT32 _TestLicCmpOffer(CONST ALM_LIC_STORE_OFFERS_STRU* pstLeft,
    CONST ALM_LIC_STORE_OFFERS_STRU* pstRight,
    OUT ALM_BOOL* pbRslt);

static ALM_INT32 _TestLicCmpSbom(CONST ALM_LIC_STORE_SBOMS_STRU* pstLeft,
    CONST ALM_LIC_STORE_SBOMS_STRU* pstRight,
    OUT ALM_BOOL* pbRslt);

static ALM_INT32 _TestLicCmpSbomValue(CONST ALM_LIC_STORE_SBOM_VALUES_STRU* pstLeft,
    CONST ALM_LIC_STORE_SBOM_VALUES_STRU* pstRight,
    OUT ALM_BOOL* pbRslt);

static ALM_INT32 _TestLicCmpPackageItem(CONST ALM_LIC_STORE_PACKAGEITEMS_STRU* pstLeft,
                                  CONST ALM_LIC_STORE_PACKAGEITEMS_STRU* pstRight,
                                  OUT ALM_BOOL* pbRslt);

static ALM_INT32 _TestLicCmpPackageItemValue(CONST ALM_LIC_STORE_PACKAGEITEM_VALUES_STRU* pstLeft,
    CONST ALM_LIC_STORE_PACKAGEITEM_VALUES_STRU* pstRight,
    OUT ALM_BOOL* pbRslt);

static ALM_INT32 _TestLicCmpS2Bbom(CONST ALM_LIC_STORE_S2BBOMS_STRU* pstLeft,
    CONST ALM_LIC_STORE_S2BBOMS_STRU* pstRight,
    OUT ALM_BOOL* pbRslt);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */

#endif /* __TEST_LIC_STRU_LIC_H__ */
