/*******************************************************************************
Description    : 测试多叉树存储s。
History        :
<version> <date>     <Author> <Description>
*******************************************************************************/
#include "group_store_tree.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */



/************************************************************************/
/* GROUP                                                                */
/************************************************************************/
ALM_VOID Group_StoreTree(ALM_CHAR* pGroup)
{
    STF_REG_CASE(_Case_StoreTree);
}

ALM_VOID ALMStoreTreePrint(IN ALM_STORE_TREE_STRU* pstTree);

#define CASE_STORE_TREE_NUM 100

typedef struct
{
    const char* pcDesc;
    int aiAdd[CASE_STORE_TREE_NUM];
    int iAddNum;

    int aiDel[CASE_STORE_TREE_NUM];
    int iDelNum;
}CaseStoreTreeSt;

/* 用例 */
static CaseStoreTreeSt s_stCases[] = {

    {"验证正向顺序添加、正向顺序删除",
    {1,1,1,1,1, 2,2,2,2,2, 3,3,3,3,3, 4,4,4,4,4, 5,5,5,5,5, 6,6,6,6,6}, 6 * 5,
    {1, 3, 6}, 3},

    {"验证正向顺序添加、反向顺序删除",
    {1,1,1,1,1, 2,2,2,2,2, 3,3,3,3,3, 4,4,4,4,4, 5,5,5,5,5, 6,6,6,6,6}, 6 * 5,
    {6, 3, 1}, 3},

    {"验证反向顺序添加、正向顺序删除",
    {6,6,6,6,6, 5,5,5,5,5, 4,4,4,4,4, 3,3,3,3,3, 2,2,2,2,2, 1,1,1,1,1}, 6 * 5,
    {1, 3, 6}, 3},

    {"验证反向顺序添加、反向顺序删除",
    {6,6,6,6,6, 5,5,5,5,5, 4,4,4,4,4, 3,3,3,3,3, 2,2,2,2,2, 1,1,1,1,1}, 6 * 5,
    {6, 3, 1}, 3},

    {"验证随机添加、及删除",
    {2,1,3,6,4,5, 1,3,6,4,5,2, 3,6,4,5,2,1, 6,4,5,2,1,3, 4,5,2,1,3,6}, 6 * 5,
    {1, 3, 4}, 3},

};

/************************************************************************
* 用例
************************************************************************/

/* 测试多叉树存储 */
static ALM_VOID _Case_StoreTree(ALM_VOID)
{
    int iRet;
    int idx;

    Test_Init(ALM_TRUE, ALM_FALSE, 0, 30);

    for (idx = 0; idx < sizeof(s_stCases) / sizeof(CaseStoreTreeSt); ++idx)
    {
        STF_printLog("%02d. %s。\n", idx + 1, s_stCases[idx].pcDesc);

        iRet = _Case_StoreTreeBase(s_stCases[idx].aiAdd,
            s_stCases[idx].iAddNum,
            s_stCases[idx].aiDel,
            s_stCases[idx].iDelNum);
        
        if (ALM_OK == iRet && STF_ASSERT_LAST_RSLT())
        {
            STF_printLog("    rslt: ok.\n");
        }
        else
        {
            STF_printLog("    rslt: failed!\n");
        }

        STF_ASSERT_LAB(ALM_OK == iRet, LAB_END);
    }

LAB_END:
    Test_Cls();
}

/************************************************************************
* 相关函数
************************************************************************/


/* 测试多叉树存储基函数 */
static int _Case_StoreTreeBase(int* piAdd, int iAddNum, int* piDel,
    int iDelNum)
{
    ALM_INT32 iRet = ALM_OK;
    ALM_STORE_TREE_STRU stTree = {0};
    ALM_STORE_TREE_CFG_STRU stCfg = {0};
    ALM_STORE_TREE_NODES_STRU stRslt = {0};
    int idx;
    int idx2;
    int iNum;

#define CASE_CHK_AND_CLS_RSLT(stRslt, iTempNum, piTempVal) \
    STF_ASSERT_LAB((iTempNum) == stRslt.uiNum, LAB_END);\
    STF_ASSERT_LAB(0 == iTempNum || ALM_NULL == (piTempVal) \
    || *((int*)(piTempVal)) == *(int*)stRslt.pstArray[0]->pData, LAB_END);\
    ALM_AdapterBaseMemFree(stRslt.pstArray); \
    stRslt.pstArray = ALM_NULL;


    /* 1. 创建多叉树。 */
    stCfg.bIncSame = ALM_TRUE, stCfg.uiUnitSize = sizeof(int), stCfg.usPortNum = 3;
    stCfg.stAdapterFuncs.fnGetPort = _Case_StoreTreeGetPort;
    iRet = ALM_StoreTreeInit(&stTree, &stCfg);
    STF_ASSERT_LAB(ALM_OK == iRet, LAB_END);

    /* 2. 测试添加数据。 */
    for (idx = 0; idx < iAddNum; ++idx)
    {
        iRet = ALM_StoreTreeAdd(&stTree, &piAdd[idx]);
        STF_ASSERT_LAB(ALM_OK == iRet, LAB_END);

        for (iNum = 1, idx2 = 0; idx2 < idx; ++idx2)
        {
            if (piAdd[idx2] == piAdd[idx])
            {
                ++iNum;
            }
        }

        iRet = ALM_StoreTreeGetNodes(&stTree, &piAdd[idx], &stRslt);
        STF_ASSERT_LAB(ALM_OK == iRet, LAB_END);
        CASE_CHK_AND_CLS_RSLT(stRslt, iNum, &piAdd[idx]);
    }

    STF_printLog("=====树节点打印start====\n");
    ALMStoreTreePrint(&stTree);
    STF_printLog("=====树节点打印end====\n");

    /* 验证全部成添加 */
    for (idx = 0; idx < iAddNum; ++idx)
    {
        iRet = ALM_StoreTreeGetNodes(&stTree, &piAdd[idx], &stRslt);
        STF_ASSERT_LAB(ALM_OK == iRet, LAB_END);

        for (iNum = 0, idx2 = 0; idx2 < iAddNum; ++idx2)
        {
            if (piAdd[idx2] == piAdd[idx])
            {
                ++iNum;
            }
        }

        CASE_CHK_AND_CLS_RSLT(stRslt, iNum, &piAdd[idx]);
    }

    /* 3. 测试删除数据。 */
    for (idx = 0; idx < iDelNum; ++idx)
    {
        iRet = ALM_StoreTreeDel(&stTree, &piDel[idx]);
        STF_ASSERT_LAB(ALM_OK == iRet, LAB_END);

        iRet = ALM_StoreTreeGetNodes(&stTree, &piDel[idx], &stRslt);
        STF_ASSERT_LAB(ALM_OK == iRet, LAB_END);
        CASE_CHK_AND_CLS_RSLT(stRslt, 0, ALM_NULL);
    }

    /* 验证删除后是否影响其他数据 */
    for (idx = 0; idx < iAddNum; ++idx)
    {
        iRet = ALM_StoreTreeGetNodes(&stTree, &piAdd[idx], &stRslt);
        STF_ASSERT_LAB(ALM_OK == iRet, LAB_END);

        for (idx2 = 0; idx2 < iDelNum; ++idx2)
        {
            if (piDel[idx2] == piAdd[idx])
            {
                break;
            }
        }

        if (idx2 < iDelNum) //已被删除
        {
            CASE_CHK_AND_CLS_RSLT(stRslt, 0, ALM_NULL);
        }
        else //未被删除
        {
            for (iNum = 0, idx2 = 0; idx2 < iAddNum; ++idx2)
            {
                if (piAdd[idx2] == piAdd[idx])
                {
                    ++iNum;
                }
            }

            CASE_CHK_AND_CLS_RSLT(stRslt, iNum, &piAdd[idx]);
        }
    }

    ALM_StoreTreeDelAll(&stTree);
    STF_ASSERT_LAB(0 == stTree.pstRoot, LAB_END);

LAB_END:
    ALM_StClsStoreTree(&stTree);
    ALM_AdapterBaseMemFree(stRslt.pstArray);
    return iRet;
}

/*
* 描述: 得到数据比较结果及下层遍历端口。得到的端口号必须小于端口总数目。
* pCmpL - 比较左数，一般为mark或数据。
* bFindByMark - 比较左数为mark还是数据。
* pDataR - 比较右数。
* pstRslt - 得到的比较结果。
* pEx - 初始化存储对象时指定的扩展成员。
*/
static ALM_INT32 _Case_StoreTreeGetPort(CONST ALM_VOID* pCmpL,
    ALM_BOOL bFindByMark,
    CONST ALM_VOID* pDataR,
    INOUT ALM_STORE_TREE_CMP_RSLT_STRU* pstRslt,
    CONST ALM_VOID* pEx)
{
    int* piLeft = (int*)pCmpL;
    int* piRight = (int*)pDataR;
    int iCmp;

    STF_ASSERT_RET(pDataR != ALM_NULL && pstRslt != ALM_NULL, RET_ERR);
    STF_ASSERT_RET(pstRslt->uiCap >= 3, RET_ERR);

    pstRslt->bThisIsEqual = ALM_FALSE;

    if (piLeft != ALM_NULL)
    {
        pstRslt->uiNum = 1;

        iCmp = *piLeft - *piRight;
        if (0 == iCmp)
        {
            pstRslt->bThisIsEqual = ALM_TRUE;
            pstRslt->ausNexts[0] = ALM_STORE_TREE_CMP_EQUAL;
        }
        else if (iCmp > 0)
        {
            pstRslt->ausNexts[0] = ALM_STORE_TREE_CMP_MORE;
        }
        else
        {
            pstRslt->ausNexts[0] = ALM_STORE_TREE_CMP_LESS;
        }
    }
    else
    {
        pstRslt->bThisIsEqual = ALM_TRUE;
        pstRslt->uiNum = 3;
        pstRslt->ausNexts[0] = ALM_STORE_TREE_CMP_EQUAL;
        pstRslt->ausNexts[1] = ALM_STORE_TREE_CMP_LESS;
        pstRslt->ausNexts[2] = ALM_STORE_TREE_CMP_MORE;
    }


    return RET_OK;
}


/*定义整形数组*/
typedef struct
{
    ALM_UINT32* pstArray;
    ALM_UINT32 uiCap;
    ALM_UINT32 uiNum;
} ALM_INT32_ARRAY_STRU;


/*记录深度节点内容*/
typedef struct
{
    ALM_UINT32 uiDeep;
    ALM_UINT32 uiData;
    ALM_UINT32 uiPort;
    ALM_STORE_TREE_NODE_STRU* pstFatherNode;
} ALM_TREE_DEEP_STRU;

/*记录某一深度下的所有节点*/
typedef struct
{
    ALM_TREE_DEEP_STRU* pstArray;
    ALM_UINT32 uiCap;
    ALM_UINT32 uiNum;
} ALM_TREE_DEEP_ARRAY_STRU;


/*记录总深度*/
typedef struct
{
    ALM_TREE_DEEP_ARRAY_STRU* pstArray;
    ALM_UINT32 uiCap;
    ALM_UINT32 uiNum;
} ALM_TREE_DEEP_ARRAYS_STRU;

/*查询树深度索引*/
ALM_UINT32 ALMStoreFindDeepIdx(ALM_TREE_DEEP_ARRAYS_STRU* pstDeepArrays,ALM_UINT32 uiDeep)
{
    ALM_UINT32 uiIdx;
    ALM_UINT32 uiDeepTem = -1;

    if (ALM_NULL != pstDeepArrays)
    {
        for (uiIdx = 0; uiIdx < pstDeepArrays->uiNum; uiIdx++)
        {
            if (uiDeep ==  uiIdx)
            {
                uiDeepTem = uiIdx;
                break;
            }
        }

    }

    return uiDeepTem;
}



/*输出这棵树*/
ALM_VOID ALMStoreTreePrint(IN ALM_STORE_TREE_STRU* pstTree)
{
    ALM_INT32 iRet = ALM_OK;
    ALM_STORE_TREE_NODE_STRU* pstItr = pstTree->pstRoot; 
    ALM_STORE_TREE_NODE_STRU* pstNext;
    ALM_STORE_TREE_NODES_STRU stNexts = {0};
    ALM_INT32_ARRAY_STRU stPortNum = {0};
    ALM_INT32_ARRAY_STRU stDeep = {0};
    ALM_UINT32 uiIdx;
    ALM_UINT32 uiIdx2;
    ALM_UINT32 uiPortNum = 0;
    ALM_UINT32 uiPortItr = 0;
    ALM_UINT32 uiDeepItr = 0;
    ALM_UINT32 uiDeep = 0;
    ALM_UINT32 uiDeepTem = 0;
    ALM_TREE_DEEP_STRU* pstDeepTem = ALM_NULL;
    ALM_TREE_DEEP_ARRAYS_STRU stDeepArrays = {0};
    ALM_TREE_DEEP_ARRAY_STRU* pstDeepArray = ALM_NULL;

    if (ALM_NULL != pstTree && ALM_NULL != pstItr)
    {   
        while(pstItr)
        {
            /*查找该深度对应的根*/
            uiDeepTem = ALMStoreFindDeepIdx(&stDeepArrays,uiDeepItr);
            if (uiDeepTem == -1)
            {
                ALM_ArrayObjExpandVoid_M(stDeepArrays,ALM_TREE_DEEP_ARRAY_STRU,1);
                pstDeepArray = &stDeepArrays.pstArray[stDeepArrays.uiNum];
                stDeepArrays.uiNum++;

            }
            else
            {
                pstDeepArray = &stDeepArrays.pstArray[uiDeepTem];
            }

            ALM_ArrayObjExpandVoid_M(*pstDeepArray,ALM_TREE_DEEP_STRU,1);
            pstDeepArray->pstArray[pstDeepArray->uiNum].uiData = *(ALM_UINT32*)(pstItr->pData);
            pstDeepArray->pstArray[pstDeepArray->uiNum].uiPort = uiPortItr;
            pstDeepArray->pstArray[pstDeepArray->uiNum].uiDeep = uiDeepItr;
            pstDeepArray->pstArray[pstDeepArray->uiNum].pstFatherNode = pstItr->pstLast;
            pstDeepArray->uiNum++;
                    
            /*查询下一个端口*/
            for (uiIdx = 0; uiIdx < 3; ++uiIdx)
            {
                pstNext = pstItr->apstPorts[uiIdx];
                if (pstNext != ALM_NULL)
                {                   
                    uiPortNum = uiIdx;
                    ALM_ArrayObjAddVoid_M(stPortNum,ALM_UINT32,&uiPortNum,1);

                    if (uiPortNum == 0)
                    {
                        /*保存下一个迭代的深度，等值端口和父节点深度一样*/
                        ALM_ArrayObjExpandVoid_M(stDeep,ALM_UINT32,1);
                        stDeep.pstArray[stDeep.uiNum] = uiDeepItr;
                        stDeep.uiNum++;
                        
                    }
                    else
                    {
                        /*保存下一个迭代的深度,非等值端口，为父节点深度的下一个 */
                        uiDeep = uiDeepItr +1;
                        ALM_ArrayObjExpandVoid_M(stDeep,ALM_UINT32,1);
                        stDeep.pstArray[stDeep.uiNum] = uiDeep;
                        stDeep.uiNum++;
                    }
                    ALM_ArrayObjAddVoid_M(stNexts,ALM_STORE_TREE_NODE_STRU*,&pstNext,1);
                }
            }           

            /* 向下迭代 */
            if (stNexts.uiNum > 0)
            {
                pstItr = stNexts.pstArray[--stNexts.uiNum];
                uiPortItr = stPortNum.pstArray[--stPortNum.uiNum]; 
                uiDeepItr = stDeep.pstArray[--stDeep.uiNum];
            }
            else
            {
                pstItr = ALM_NULL;
            }
            
        }

        for (uiIdx = 0; uiIdx < stDeepArrays.uiNum; uiIdx++)
        {
            pstDeepArray = &stDeepArrays.pstArray[uiIdx];            
            for (uiIdx2= 0; uiIdx2 < pstDeepArray->uiNum; uiIdx2++)
            {
                pstDeepTem = &pstDeepArray->pstArray[uiIdx2];
                if (pstDeepTem->pstFatherNode == ALM_NULL)
                {
                    STF_printLog("深度%d,值:%d,端口号：%d,父节点值：%d\n",pstDeepTem->uiDeep,pstDeepTem->uiData,pstDeepTem->uiPort,0);
                }
                else
                {
                    STF_printLog("深度%d,值:%d,端口号：%d,父节点值：%d\n",pstDeepTem->uiDeep,pstDeepTem->uiData,pstDeepTem->uiPort,*((ALM_UINT32*)pstDeepTem->pstFatherNode->pData));
                }               
            }
        }

    }

    for (uiIdx = 0; uiIdx < stDeepArrays.uiNum;uiIdx++)
    {
        ALM_ArrayObjCls0_M(stDeepArrays.pstArray[uiIdx],ALM_TREE_DEEP_STRU);
    }
    ALM_ArrayObjCls0_M(stDeepArrays,ALM_TREE_DEEP_ARRAY_STRU);
    ALM_ArrayObjCls0_M(stNexts,ALM_STORE_TREE_NODE_STRU*);
    ALM_ArrayObjCls0_M(stPortNum,ALM_UINT32*);
    ALM_ArrayObjCls0_M(stDeep,ALM_UINT32*);

}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */
