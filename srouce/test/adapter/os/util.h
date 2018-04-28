#ifndef _util_h_
#define _util_h_

#include "def.h"
#include <stdio.h>
#include <stdlib.h>
#include "Tst_Framework.h"

/***************************************************************************
* 描述: 函数。
***************************************************************************/

//转换字符串为对应的类型描述。
TypeEn Util_ChangStrToType(CONST ALM_CHAR* pcType);

//读取文件到缓存中，需要外部释放缓存內存。
ALM_INT32 Util_ReadFileToBuffer(CONST ALM_CHAR* pcFile, ALM_CHAR** ppcBuf, ALM_INT32* pulReadLen);

//忽略大小写比较字符串。
ALM_INT32 Util_StrCmpIgrUp(CONST ALM_CHAR* pcLeft, CONST ALM_CHAR* pcRight);

//忽略大小写比较字符串前N个字符。
ALM_INT32 Util_StrNCmpIgrUp(CONST ALM_CHAR* pcLeft, CONST ALM_CHAR* pcRight, ALM_INT32 iCount);

//拷贝字符串。
ALM_CHAR* Util_StrCpy(CONST ALM_CHAR* pcSrc);

//拷贝字符串。
ALM_CHAR* Util_StrNCpy(CONST ALM_CHAR* pcSrc, ALM_INT32 iNum);

//校验字符串是否为空
bool Util_StrChkEmpty(CONST ALM_CHAR* pcStr);

//拷贝字符串。
bool Util_StrNGetLong(CONST ALM_CHAR* pcSrc, ALM_INT32 iStrlen, ALM_INT64* plRet);

//将字符串转换为数字。
bool Util_StrGetLong(CONST ALM_CHAR* pcSrc, ALM_INT64* plRet);

//判断文件是否存在，如果不存在依据指定创建文件。
bool Util_FileChk(CONST ALM_CHAR* pcFileName, bool bNeedCreate);

//检验参数是否为空再释放内存。
ALM_VOID Util_Free(ALM_VOID* pMem);

//在数组尾部扩展数组，返回当前结点数目。额外增加一个空数据结点。
ALM_INT32 Util_ArrayExpand(ALM_VOID** ppArray,
                     ALM_UINT32 uiUnitSize,
                     ALM_UINT32 uiCurNum,
                     ALM_UINT32 uiExpand);

//在数组尾部扩展数组，返回当前结点数目。
ALM_INT32 Util_ArrayExpandAuto(ALM_VOID** ppArray,
                     ALM_UINT32 uiUnitSize,
                     ALM_UINT32 uiCurNum,
                     INOUT ALM_UINT32* puiCap);

/*****************************************************************************
* 链表操作。
*****************************************************************************/

/* 链表结点必须包含的成员 */
#define TL_LIST_NODE_MEBR(type) type* pstLast; type* pstNext

/* 定义链表类型 */
#define TL_LIST_DIM(type) \
    struct {type* pstHead; type* pstTail; unsigned int uiTotal; }

/* 添加一个结点，直接添加结点指针。若指定pstLast为空，在头部添加 */
#define TL_ListAdd_M(stList, pstNode, pstLastNode) \
    {\
        (pstNode)->pstLast = (pstLastNode);\
        if ((pstNode)->pstLast != NULL)\
        {\
            (pstNode)->pstNext = (pstNode)->pstLast->pstNext;\
            (pstNode)->pstLast->pstNext = (pstNode);\
        }\
        else\
        {\
            (pstNode)->pstNext = (stList).pstHead;\
            (stList).pstHead = (pstNode);\
        }\
        if (pstNode->pstNext != NULL)\
        {\
            (pstNode)->pstNext->pstLast = (pstNode);\
        }\
        else\
        {\
            (stList).pstTail = (pstNode);\
        }\
        ++(stList).uiTotal;\
    }


/* 删除一个结点，不释放结点本身所占用的空间 */
#define TL_ListDel_M(stList, pstNode) \
    {\
        if ((pstNode)->pstLast != NULL)\
        {\
            (pstNode)->pstLast->pstNext = (pstNode)->pstNext;\
        }\
        else\
        {\
            (stList).pstHead = (pstNode)->pstNext;\
        }\
        if ((pstNode)->pstNext != NULL)\
        {\
            (pstNode)->pstNext->pstLast = (pstNode)->pstLast;\
        }\
        else\
        {\
            (stList).pstTail = (pstNode)->pstLast;\
        }\
        --(stList).uiTotal;\
    }


#endif // _util_h_
