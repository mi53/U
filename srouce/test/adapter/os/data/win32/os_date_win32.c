#include "os.h"

#if (LIC_OS == LIC_WINDOWS)

#include <Windows.h>

/***************************************************************************
* 描述: 获取系统时间。
***************************************************************************/
ALM_INT32 OS_DateGet(OsDateSt *pstDate)
{
    LPSYSTEMTIME lpSystemTime;

    lpSystemTime = (SYSTEMTIME*)OS_MemAlloc_M(sizeof(SYSTEMTIME));
    if (ALM_NULL == lpSystemTime)
    {
        return RET_ERR;
    }

    GetLocalTime(lpSystemTime);

    pstDate->uwYear = (ALM_UINT16)lpSystemTime->wYear;
    pstDate->ucMonth = (ALM_UCHAR)lpSystemTime->wMonth;
    pstDate->ucDate = (ALM_UCHAR)lpSystemTime->wDay;
    pstDate->ucHour = (ALM_UCHAR)lpSystemTime->wHour;
    pstDate->ucMinute = (ALM_UCHAR)lpSystemTime->wMinute;
    pstDate->ucSecond = (ALM_UCHAR)lpSystemTime->wSecond;
    pstDate->ucWeek = (ALM_UCHAR)lpSystemTime->wDayOfWeek;

    OS_MemFree_M(lpSystemTime);

    return RET_OK;
}

#endif /* OS */
