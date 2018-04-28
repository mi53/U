/*******************************************************************************
Description    : 测试多叉树存储。
History        :
<version> <date>     <Author> <Description>
*******************************************************************************/
#ifndef __GROUP_STORE_TREE_H__
#define __GROUP_STORE_TREE_H__

#include "test_common.h"
#include "alm_store_tree.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */


/************************************************************************/
/* GROUP                                                                */
/************************************************************************/
ALM_VOID Group_StoreTree(ALM_CHAR* pGroup);

/************************************************************************
* 用例
************************************************************************/

/* 测试多叉树存储 */
static ALM_VOID _Case_StoreTree(ALM_VOID);

/************************************************************************
* 内部定义
************************************************************************/

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
    CONST ALM_VOID* pEx);

/* 测试多叉树存储基函数 */
static int _Case_StoreTreeBase(int* piAdd, int iAddNum, int* piDel,
    int iDelNum);


#ifdef __cplusplus
#if __cplusplus
}
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */

#endif /* __GROUP_STORE_TREE_H__ */
