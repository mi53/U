#include "def.h"

typedef struct _OsDateSt
{
   /*if set to OS time the scope is 1970 ~ 2038, or
    the scope is 1970 ~ 2100
    */
    ALM_UINT16 uwYear;

   /* scope is 1 - 12 */
    ALM_UCHAR ucMonth;

    /* scope is 1 - 31 */
    ALM_UCHAR ucDate;

    /* scope is 0 - 23 */
    ALM_UCHAR ucHour;

    /* scope is 0 - 59 */
    ALM_UCHAR ucMinute;

    /* scope is 0 - 59 */
    ALM_UCHAR ucSecond;

    /* scope is 0 - 6  */
    ALM_UCHAR ucWeek;
}OsDateSt;

/* 配置模拟时间的初始时间，若为ALM_NULL，使用系统时间 */
#define OS_DATA_SIMULA_INIT_TIME "1999-1-1 12:0:0"

/***************************************************************************
* 描述: 需要适配的函数.
***************************************************************************/

//获取系统时间。
ALM_INT32 OS_DateGet(OsDateSt *pstDate);


/***************************************************************************
* 描述: 其他工具类函数。
***************************************************************************/

//获取系统时间，并以字符串形式返回。
CONST ALM_CHAR* OS_DateStr();

//启动日期模拟器。需要先初始化内存及互斥量模块。
ALM_VOID OS_DateSimulaStart();

//关闭日期模拟器。
ALM_VOID OS_DateSimulaClose();

//获取当前的模拟时间。
ALM_INT32 OS_DateSimulaGet(OsDateSt *pstDate);

//以字符串形式返回当前模拟时间。
CONST ALM_CHAR* OS_TimeSimulaStr();

//增加指定秒数到模拟时间。
CONST ALM_CHAR* OS_DateSimulaAddSecs(ALM_INT32 iSecs);

//模拟时间往前调整。
CONST ALM_CHAR* OS_DateSimulaReduceSecs(ALM_INT32 iSecs);

//设置模拟时间。
ALM_INT32 OS_DateSimulaSet(OsDateSt* pstNewTime);

//设置模拟时间。
ALM_INT32 OS_DateSimulaSetByStr(CONST ALM_CHAR* pcData);

//阻塞计时。
ALM_VOID OS_DateSimulaBlock();

//恢复计时。
ALM_VOID OS_DateSimulaRecover();

/***************************************************************************
* 描述: 内部函数。
***************************************************************************/

//将时间转变为整数。
static ALM_INT32 _OS_DateToInt(CONST OsDateSt* pstDate, OUT ALM_INT64* plInt);

//将整数转变为时间。
static ALM_INT32 _OS_DateGetByInt(OUT OsDateSt* pstDate, ALM_INT64 ulInt);

//定时器自动累加时间。
static ALM_VOID _OS_DateSimulaAuto(ALM_VOID* pPara);
