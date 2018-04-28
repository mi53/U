#include "util.h"
#include "os.h"

#include <string.h>
#include <stdarg.h>


//转换字符串为对应的类型描述。
TypeEn Util_ChangStrToType(CONST ALM_CHAR* pcType)
{
    ALM_INT32 iCmp;

    iCmp = Util_StrCmpIgrUp(pcType, "TYPE_INT");
    if (0 == iCmp)
        return TYPE_INT;

    iCmp = Util_StrCmpIgrUp(pcType, "TYPE_STR");
    if (0 == iCmp)
        return TYPE_STR;

    return TYPE_UNKNOW;
}

//读取文件到缓存中，需要外部释放缓存內存。
ALM_INT32 Util_ReadFileToBuffer(CONST ALM_CHAR* pcFile, ALM_CHAR** ppcBuf, ALM_INT32* pulReadLen)
{
    FILE* pLk = ALM_NULL;
    ALM_INT32 iLkLen = 0;
    ALM_INT32 iLen;

     //参数检测
    if (ALM_NULL == pcFile || ALM_NULL == ppcBuf)
    {
        OS_LOG_INFO("Null parameter!");
        return RET_ERR;
    }

    //初始化输出
    *ppcBuf = ALM_NULL;

    if (ALM_NULL != pulReadLen)
        *pulReadLen = 0;

    /************************************************************
    * 1. 打开文件
    ************************************************************/
    pLk = fopen(pcFile, "rb");
    if (ALM_NULL == pLk)
    {
        OS_LOG_ERR("file %s is not exist.", pcFile);
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
    *ppcBuf = (ALM_CHAR*)malloc(iLkLen + 1);
    if (ALM_NULL == *ppcBuf)
    {
        OS_LOG_ERR("Malloc fail, malloc size is %d.", iLkLen + 1);
        fclose(pLk);
        Util_Free(*ppcBuf);
        *ppcBuf = ALM_NULL;

        return RET_ERR;
    }
    memset(*ppcBuf, 0, iLkLen + 1);

    /************************************************************
    * 3. 读取文件内容
    ************************************************************/
    iLen = fread(*ppcBuf, sizeof(ALM_CHAR), iLkLen, pLk);
    if (iLen != iLkLen)
    {
        OS_LOG_ERR("Need read %d bytes, but only read %d bytes.",
            iLkLen,
            iLen);
        fclose(pLk);
        Util_Free(*ppcBuf);
        *ppcBuf = ALM_NULL;

        return RET_ERR;
    }
    if (ALM_NULL != pulReadLen)
        *pulReadLen = iLen;

    /************************************************************
    * 4. 关闭文件
    ************************************************************/
    fclose(pLk);

    return RET_OK;
}

//忽略大小写比较字符串。
ALM_INT32 Util_StrCmpIgrUp(CONST ALM_CHAR* pcLeft, CONST ALM_CHAR* pcRight)
{
    ALM_INT32 index = 0;
    ALM_INT32 iUpOffset = 'A' - 'a';

    #define UTIL_STR_CMP_MAX_LEN 1024*1024*100

    #define _TST_IS_SYB(x) \
        ((x >= 'A' && x <= 'Z') || (x >= 'a' && x <= 'z'))

    #define _TST_CH_EQ_IGRUP(ch1, ch2) \
        (ch1 == ch2) || (_TST_IS_SYB(ch1) && _TST_IS_SYB (ch2) \
        && (ch1 - ch2 == iUpOffset || ch2 - ch1 == iUpOffset))

    if (pcLeft == pcRight && ALM_NULL != pcLeft)
    {
        return 0;
    }
    else if (ALM_NULL == pcLeft || ALM_NULL == pcRight)
    {
        OS_LOG_ERR("传入空指针进行字符串比较。");
        return -1;
    }
   
    while (index < UTIL_STR_CMP_MAX_LEN)
    {
        if (_TST_CH_EQ_IGRUP(pcLeft[index], pcRight[index]))
        {
            if ('\0' == pcLeft[index])
                return 0;
            ++index;
        }
        else
        {
            return pcLeft[index] - pcRight[index];
        }
    }
    
    OS_LOG_ERR("超长字符串。");
    return -1;
}

//忽略大小写比较字符串前N个字符。
ALM_INT32 Util_StrNCmpIgrUp(CONST ALM_CHAR* pcLeft, CONST ALM_CHAR* pcRight, ALM_INT32 iCount)
{
    ALM_CHAR* pcBufLeft;
    ALM_CHAR* pcBufRight;
    ALM_INT32 iCmp;

    if (ALM_NULL == pcLeft || ALM_NULL == pcRight || iCount <= 0)
    {
        OS_LOG_ERR("字符串比较传入无效参数。");
        return -1;
    }

    pcBufLeft = (ALM_CHAR*)malloc(iCount + 1);
    pcBufRight = (ALM_CHAR*)malloc(iCount + 1);

    memset(pcBufLeft, 0, iCount + 1);
    memset(pcBufRight, 0, iCount + 1);

    strncpy(pcBufLeft, pcLeft, iCount);
    strncpy(pcBufRight, pcRight, iCount);

    iCmp = Util_StrCmpIgrUp(pcBufLeft, pcBufRight);

    Util_Free(pcBufLeft);
    Util_Free(pcBufRight);

    return iCmp;
}

//拷贝字符串。
ALM_CHAR* Util_StrCpy(CONST ALM_CHAR* pcSrc)
{
    ALM_CHAR* pcRet;
    ALM_INT32 iLen;

    if (ALM_NULL == pcSrc)
    {
        return ALM_NULL;
    }
    
    /***********************************************************
    * 获取字符串长度。不考虑结尾符号
    ***********************************************************/
    iLen = -1;
    while ('\0' != pcSrc[++iLen]);
    
    /***********************************************************
    * 拷贝
    ***********************************************************/
    pcRet = (ALM_CHAR*)malloc(iLen + 1);
    memcpy(pcRet, pcSrc, iLen);
    pcRet[iLen] = '\0';

    return pcRet;
}

//拷贝字符串。
ALM_CHAR* Util_StrNCpy(CONST ALM_CHAR* pcSrc, ALM_INT32 iNum)
{
    ALM_INT32 index;
    ALM_CHAR* pcRet;

    if (ALM_NULL == pcSrc)
    {
        return ALM_NULL;
    }
    
    /***********************************************************
    * 获取字符串长度。
    ***********************************************************/
    for (index = 0; index < iNum; ++index)
    {
        if ('\0' == pcSrc[index])
        {
            break;
        }
    }

    //计算长度，不考虑结尾符号。
    if (index != iNum)
    {
        iNum = index;
    }
    
    /***********************************************************
    * 拷贝
    ***********************************************************/
    pcRet = (ALM_CHAR*)malloc(iNum + 1);
    memcpy(pcRet, pcSrc, iNum);
    pcRet[iNum] = '\0';

    return pcRet;
}


//校验字符串是否为空
bool Util_StrChkEmpty(CONST ALM_CHAR* pcStr)
{
    bool bRet = true;

    if (ALM_NULL == pcStr)
    {
        return true;
    }
    
    while ('\0' != *pcStr)
    {
        if ('\n' != *pcStr && ' ' != *pcStr && '\r' != *pcStr && '\t' != *pcStr)
        {
            bRet = false;
            break;
        }
        ++pcStr;
    }

    return bRet;
}

bool Util_StrNGetLong(CONST ALM_CHAR* pcSrc, ALM_INT32 iStrlen, ALM_INT64* plRet)
{
    ALM_CHAR* pcTmp = ALM_NULL;
    bool bRet;

    if (iStrlen > 0)
        pcTmp = Util_StrNCpy(pcSrc, iStrlen);

    if (ALM_NULL == pcTmp)
        return false;
    
    bRet = Util_StrGetLong(pcTmp, plRet);
    Util_Free(pcTmp);

    return bRet;
}

//将字符串转换为数字。
bool Util_StrGetLong(CONST ALM_CHAR* pcSrc, ALM_INT64* plRet)
{
    ALM_INT32 index = 0;
    bool bIsNeg = false;

    OS_CHK(ALM_NULL != pcSrc && ALM_NULL != plRet);

    *plRet = 0;
    while (pcSrc[index] != '\0')
    {
        if (pcSrc[index] >= '0' && pcSrc[index] <= '9')
        {
            *plRet *= 10;
            *plRet += (pcSrc[index] - '0');
        }
        else if (0 == index && '-' == pcSrc[index])
        {
            bIsNeg = true;
        }
        else if (' ' == pcSrc[index] || '\t' == pcSrc[index])
        {
            ++index;
            continue;
        }
        else
        {
            return false;
        }
        
        
        ++index;
    }

    if (bIsNeg)
    {
        *plRet *= -1;
    }
    
    return true;
}

//判断文件是否存在，如果不存在依据指定创建文件。
bool Util_FileChk(CONST ALM_CHAR* pcFileName, bool bNeedCreate)
{
    FILE* pFile = ALM_NULL;

    if(ALM_NULL == pcFileName)
        return false;

    pFile = fopen(pcFileName, "rb");
    if (ALM_NULL == pFile && bNeedCreate)
    {
        //保证不截断现有文件。
        pFile = fopen(pcFileName, "ab+");
    }
    
    if (ALM_NULL != pFile)
    {
        fclose(pFile);
        return true;
    }

    return false;
}


//检验参数是否为空再释放内存。
ALM_VOID Util_Free(ALM_VOID* pMem)
{
    if (ALM_NULL != pMem)
    {
        free(pMem);
    }
}


//在数组尾部扩展数组，返回当前结点数目。额外增加一个空数据结点。
ALM_INT32 Util_ArrayExpand(ALM_VOID** ppArray,
                     ALM_UINT32 uiUnitSize,
                     ALM_UINT32 uiCurNum,
                     ALM_UINT32 uiExpand)
{
    ALM_VOID* pTmp = ALM_NULL;
    ALM_UINT32 uiSize = 0;

    if (ALM_NULL == ppArray || 0 == uiExpand || 0 == uiUnitSize)
    {
        OS_LOG_ERR("ppArray is null.");
        return uiCurNum;
    }

    /* 分配新空间，额外扩展一个空白结点 */
    uiSize = uiUnitSize * (uiCurNum + uiExpand + 1);
    pTmp = malloc(uiSize);
    if (ALM_NULL == pTmp)
    {
        return uiCurNum;
    }
    (ALM_VOID)memset(pTmp, 0, uiSize);

    /* 更新数据 */
    if (uiCurNum > 0)
    {
        (ALM_VOID)memcpy(pTmp, *ppArray, uiUnitSize * uiCurNum);
    }

    Util_Free(*ppArray);
    *ppArray = pTmp;

    return uiCurNum + uiExpand;
}



//在数组尾部扩展数组，返回当前结点数目。
ALM_INT32 Util_ArrayExpandAuto(ALM_VOID** ppArray,
                         ALM_UINT32 uiUnitSize,
                         ALM_UINT32 uiCurNum,
                         INOUT ALM_UINT32* puiCap)
{
    ALM_UINT32 uiExpand = 0;
    ALM_INT32 iRet;
    
    #define UTIL_ARRAY_AUTO_EXPAND_NUM 10

    if (ALM_NULL == ppArray || ALM_NULL == puiCap)
    {
        OS_LOG_ERR("null para.");
        return uiCurNum;
    }
    
    if (*puiCap > uiCurNum)
    {
        return uiCurNum + 1;
    }
    
    uiExpand = *puiCap - uiCurNum;
    if (uiExpand < UTIL_ARRAY_AUTO_EXPAND_NUM)
    {
        uiExpand = UTIL_ARRAY_AUTO_EXPAND_NUM;
    }
    
    #undef UTIL_ARRAY_AUTO_EXPAND_NUM

    iRet = Util_ArrayExpand(ppArray, uiUnitSize, uiCurNum, uiExpand);

    *puiCap += uiExpand;
    
    return iRet;
}
