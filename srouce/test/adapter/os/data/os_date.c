#include "os.h"
#include <string.h>
#include <stdio.h>



/***************************************************************************
* 描述: 全局变量定义。
***************************************************************************/
static ALM_UINT32 s_uiDateLock;
static OsDateSt s_stSimulaDate;

//是否阻塞当前模拟时间自动计时。
static bool s_bBlock = false;

#define OS_DATE_SIMULA_LOCK "OS_DATE_SIMULA_LOCK"
#define OS_DATE_SIMULA_TIMER_ID (-1)

/* 若为ALM_NULL使用系统时间 */
static CONST ALM_CHAR* s_pcInitTime = OS_DATA_SIMULA_INIT_TIME; 

/***************************************************************************
* 描述: 获取系统时间。
***************************************************************************/
CONST ALM_CHAR* OS_DateStr()
{
    static ALM_CHAR buf[32] = {0};
    OsDateSt stTime;

    OS_DateGet(&stTime);
    sprintf(buf,
        "%04d-%02d-%02d %02d:%02d:%02d",
        stTime.uwYear, 
        stTime.ucMonth, 
        stTime.ucDate, 
        stTime.ucHour, 
        stTime.ucMinute, 
        stTime.ucSecond);

    return buf;
}


/***************************************************************************
* 描述: 模拟时间相关操作。
***************************************************************************/

//启动日期模拟器。
ALM_VOID OS_DateSimulaStart()
{
    OsDateSt stData;
    ALM_UINT32 uiLock = (ALM_UINT32)s_uiDateLock;
    OS_LockCreate(OS_DATE_SIMULA_LOCK, &uiLock);

    /* 初始化系统时间处理 */
    if (s_pcInitTime != ALM_NULL)
    {
        OS_DateSimulaSetByStr(s_pcInitTime);
    }
    else
    {
        OS_DateGet(&stData);
        OS_DateSimulaSet(&stData);
    }

    /* 模拟时间使用定时器自动刷新 */
    OS_TimerNew(OS_DATE_SIMULA_TIMER_ID,
        OS_TIMER_CORE_CRYCLE_SEC * 1000,
        _OS_DateSimulaAuto,
        ALM_NULL, ALM_NULL);

}


//关闭日期模拟器。
ALM_VOID OS_DateSimulaClose()
{
    OS_TimerDel((ALM_UINT32)OS_DATE_SIMULA_TIMER_ID);
}


//获取当前的模拟时间。
ALM_INT32 OS_DateSimulaGet(OsDateSt *pstDate)
{
    if (ALM_NULL == pstDate)
    {
        return RET_ERR;
    }
    
    memcpy(pstDate, &s_stSimulaDate, sizeof(s_stSimulaDate));

    return RET_OK;
}

//以字符串形式返回当前模拟时间。
CONST ALM_CHAR* OS_TimeSimulaStr()
{
    static ALM_CHAR buf[32] = {0};

    sprintf(buf,
        "%04d-%02d-%02d %02d:%02d:%02d",
        s_stSimulaDate.uwYear, 
        s_stSimulaDate.ucMonth, 
        s_stSimulaDate.ucDate, 
        s_stSimulaDate.ucHour, 
        s_stSimulaDate.ucMinute, 
        s_stSimulaDate.ucSecond);

    return buf;
}

//增加指定秒数到模拟时间。
CONST ALM_CHAR* OS_DateSimulaAddSecs(ALM_INT32 iSecs)
{
    ALM_INT32 iRet;
    ALM_INT64 lInterval;
    OsDateSt stCnt;

    OS_LockP(s_uiDateLock, 0);

    iRet = OS_DateSimulaGet(&stCnt);
    if (iRet != RET_OK)
    {
        return ALM_NULL;
    }

    _OS_DateToInt(&stCnt, &lInterval);
    lInterval += iSecs;

    _OS_DateGetByInt(&stCnt, lInterval);

    OS_DateSimulaSet(&stCnt);

    OS_LockV(s_uiDateLock);

    return OS_TimeSimulaStr();
}

//模拟时间往前调整。
CONST ALM_CHAR* OS_DateSimulaReduceSecs(ALM_INT32 iSecs)
{
    return OS_DateSimulaAddSecs(iSecs * -1);
}

//设置模拟时间。
ALM_INT32 OS_DateSimulaSet(OsDateSt* pstNewTime)
{
    //简单校验时间有效性。
    if (ALM_NULL == pstNewTime
        || pstNewTime->ucMonth > 12
        || pstNewTime->ucDate > 31
        || pstNewTime->ucHour > 24
        || pstNewTime->ucMinute > 60
        || pstNewTime->ucSecond > 60)
    {
        return RET_ERR;
    }

    //更新时间。
    if (0 != pstNewTime)
    {
        OS_LockP(s_uiDateLock, 0);

        memcpy(&s_stSimulaDate, pstNewTime, sizeof(OsDateSt));

        OS_LockV(s_uiDateLock);

        return RET_OK;
    }

    return RET_ERR;
}

//设置模拟时间。
ALM_INT32 OS_DateSimulaSetByStr(CONST ALM_CHAR* pcData)
{
    OsDateSt stNewTime;
    ALM_INT32 iYear, iMonth, iDay, iHour = 0, iMin = 0, iSec = 0;
    ALM_INT32 iRet = RET_OK;

    /* 字符串时间转换为结构体时间 */
    iRet = sscanf(pcData, "%d-%d-%d %d:%d:%d",
        &iYear, &iMonth, &iDay, &iHour, &iMin, &iSec);
    if (iRet != 6 && iRet != 3)
    {
        iRet = sscanf(pcData, "%d/%d/%d %d:%d:%d",
            &iYear, &iMonth, &iDay, &iHour, &iMin, &iSec);
        if (iRet != 6 && iRet != 3)
        {
            return RET_ERR;
        }
    }

    stNewTime.uwYear = (ALM_UINT16)iYear;
    stNewTime.ucMonth = (ALM_UCHAR)iMonth;
    stNewTime.ucDate = (ALM_UCHAR)iDay;
    stNewTime.ucHour = (ALM_UCHAR)iHour;
    stNewTime.ucMinute = (ALM_UCHAR)iMin;
    stNewTime.ucSecond = (ALM_UCHAR)iSec;

    iRet = OS_DateSimulaSet(&stNewTime);

    return iRet;
}

/***************************************************************************
* 描述: 内部函数。
***************************************************************************/

#define ALM_TIME_MAX_YEAR 10000
#define ALM_TIME_MAX_MONTH 13
#define ALM_TIME_MAX_DATE 32
#define ALM_TIME_MAX_HOUR 24
#define ALM_TIME_MAX_MIN 60
#define ALM_TIME_MAX_SEC 60
#define ALM_TIME_LEAP_MONTH 2

#define ALM_TIME_ONE_MIN_SECS 60
#define ALM_TIME_ONE_HOUR_SECS (60 * 60)
#define ALM_TIME_ONE_DAY_SECS (24 * 60 * 60)
#define ALM_TIME_ONE_DAY_MINS (24 * 60)
#define ALM_TIME_ONE_HOUR_MINS (60)

/* Leap year month days */
static ALM_INT32 s_aulAlmMonths[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

#define ALM_TIME_IS_LEAP_YEAR(year) \
    (((0 == (year) % 4) && (0 != (year) % 100)) || (0 == (year) % 400))

#define ALM_TIME_ONE_MONTH_DAYS(year, month) ((ALM_UINT32) \
    ((ALM_TIME_IS_LEAP_YEAR(year) && (2 == (month))) ? 29 : s_aulAlmMonths[month-1]))
#define ALM_TIME_ONE_YEAR_DAYS(year) ((ALM_UINT32) \
    (ALM_TIME_IS_LEAP_YEAR(year) ? 366 : 365))

#define ALM_TIME_ONE_MONTH_SECS(year, month) ((ALM_UINT32) \
    (ALM_TIME_ONE_MONTH_DAYS(year, month) * ALM_TIME_ONE_DAY_SECS))
#define ALM_TIME_ONE_YEAR_SECS(year)  ((ALM_UINT32) \
    ((ALM_TIME_IS_LEAP_YEAR(year) ? 366 : 365) * ALM_TIME_ONE_DAY_SECS))

/* 将日期转换为整数的基准时间，必须为1月1日 */
CONST static OsDateSt s_stBase = {2010, 1, 1};

/* 获取时间间隔，参数plIntervalSecs将得到pstLeft减pstRight所得的秒数。 */
static ALM_INT32 _OS_DateToInt(CONST OsDateSt* pstDate, OUT ALM_INT64* plInt)
{
    ALM_UINT16 usIdx;
    ALM_INT64 lSecsL = 0;
    ALM_INT64 lSecsR = 0;
    ALM_UCHAR ucIdx;

    if (ALM_NULL == pstDate || ALM_NULL == plInt)
    {
        return RET_ERR;
    }

    /* 获取当年内已流逝的秒数 */
    for (ucIdx = 1; ucIdx < s_stBase.ucMonth; ++ucIdx)
    {
        lSecsR += ALM_TIME_ONE_MONTH_SECS(s_stBase.uwYear, ucIdx);
    }
    lSecsR += ((s_stBase.ucDate - 1) * ALM_TIME_ONE_DAY_SECS);
    lSecsR += (s_stBase.ucHour * ALM_TIME_ONE_HOUR_SECS);
    lSecsR += (s_stBase.ucMinute * ALM_TIME_ONE_MIN_SECS);
    lSecsR += s_stBase.ucSecond;

    for (ucIdx = 1; ucIdx < pstDate->ucMonth; ++ucIdx)
    {
        lSecsL += ALM_TIME_ONE_MONTH_SECS(pstDate->uwYear, ucIdx);
    }
    lSecsL += ((pstDate->ucDate - 1) * ALM_TIME_ONE_DAY_SECS);
    lSecsL += (pstDate->ucHour * ALM_TIME_ONE_HOUR_SECS);
    lSecsL += (pstDate->ucMinute * ALM_TIME_ONE_MIN_SECS);
    lSecsL += pstDate->ucSecond;

    *plInt = lSecsL - lSecsR;

    /* 获取年份差额 */
    if (s_stBase.uwYear < pstDate->uwYear)
    {
        for (usIdx = s_stBase.uwYear; usIdx < pstDate->uwYear; ++usIdx)
        {
            *plInt += ALM_TIME_ONE_YEAR_SECS(usIdx);
        }
    }
    else if (s_stBase.uwYear > pstDate->uwYear)
    {
        for (usIdx = pstDate->uwYear; usIdx < s_stBase.uwYear; ++usIdx)
        {
            *plInt -= ALM_TIME_ONE_YEAR_SECS(usIdx);
        }
    }

    return RET_OK;
}

//将整数转变为时间。
static ALM_INT32 _OS_DateGetByInt(OUT OsDateSt* pstDate, ALM_INT64 lInt)
{
    ALM_UCHAR ucIdx;

    if (ALM_NULL == pstDate)
    {
        return RET_ERR;
    }
    
    memcpy(pstDate, &s_stBase, sizeof(s_stBase));

    /* 更新年份 */
    if (lInt > 0)
    {
        while (lInt >= ALM_TIME_ONE_YEAR_SECS(pstDate->uwYear))
        {
            lInt -= ALM_TIME_ONE_YEAR_SECS(pstDate->uwYear);
            ++pstDate->uwYear;
        }
    }
    else
    {
        while (lInt < 0)
        {
            lInt += ALM_TIME_ONE_YEAR_SECS(pstDate->uwYear - 1);
            --pstDate->uwYear;
        }
    }

    /* 更新月份 */
    ucIdx = 1;
    while (lInt >= ALM_TIME_ONE_MONTH_SECS(pstDate->uwYear, ucIdx))
    {
        lInt -= ALM_TIME_ONE_MONTH_SECS(pstDate->uwYear, ucIdx);
        ++ucIdx;
    }
    pstDate->ucMonth = ucIdx;

    /* 更新时分秒 */
    pstDate->ucDate = (ALM_UCHAR)(lInt / ALM_TIME_ONE_DAY_SECS + 1);
    lInt %= ALM_TIME_ONE_DAY_SECS;

    pstDate->ucHour = (ALM_UCHAR)(lInt / ALM_TIME_ONE_HOUR_SECS);
    lInt %= ALM_TIME_ONE_HOUR_SECS;

    pstDate->ucMinute = (ALM_UCHAR)(lInt / ALM_TIME_ONE_MIN_SECS);
    lInt %= ALM_TIME_ONE_MIN_SECS;

    pstDate->ucSecond = (ALM_UCHAR)lInt;

    return ALM_OK;
}

//定时器自动累加时间。
static ALM_VOID _OS_DateSimulaAuto(ALM_VOID* pPara)
{
    (ALM_VOID)pPara;
    if (!s_bBlock)
    {
        OS_DateSimulaAddSecs(OS_TIMER_CORE_CRYCLE_SEC);
    }
}

//阻塞计时。
ALM_VOID OS_DateSimulaBlock()
{
    s_bBlock = true;
}

//恢复计时。
ALM_VOID OS_DateSimulaRecover()
{
    s_bBlock = false;
}
