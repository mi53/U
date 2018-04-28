/**************************************************************************
    filename :  src\base\os_adapter\os.h
    file path:  src\base\os_adapter
    file base:  os
    file ext :  h
    
    purpose  :  适配不同系统函数的对外头文件。
***************************************************************************/

#ifndef _os_h_
#define _os_h_

#include "def.h"
#include "util.h"
#include "Tst_Framework.h"

/***************************************************************************
* 描述: 编译选择操作系统。
***************************************************************************/
#define LIC_WINDOWS 1
#define LIC_LINUX   2

#if defined(_WIN32)
#define LIC_OS LIC_WINDOWS

#elif defined(LINUX)
#define LIC_OS LIC_LINUX
#else
#error 不支持的操作系统。
#endif

/***************************************************************************
* 描述: 一些简单的系统函数定义。
***************************************************************************/
#if (LIC_OS == LIC_WINDOWS)
#include <stdlib.h>
#include <direct.h>
#include <io.h>

#define putenv _putenv
#define snprintf _snprintf
#define getcwd _getcwd
#define access _access
#define sleep Sleep

#else
#include <unistd.h>
//#include <dir.h>

#endif

/***************************************************************************
* 描述: OS子组件。
***************************************************************************/
#include "os_memory.h"
#include "os_date.h"
#include "os_mutex.h"
#include "os_timer.h"
#include "os_thread.h"

/***************************************************************************
* 描述: OS启动。
***************************************************************************/
ALM_INT32 OS_Start();

/***************************************************************************
* 描述: OS关闭。
***************************************************************************/
ALM_INT32 OS_Close();

/************************************************************************/
/* OS日志。                                                             */
/************************************************************************/
#define OS_LOG_INFO(...) STF_printLog("[提示] " __VA_ARGS__), STF_printLog("\n")
#define OS_LOG_ERR(...) STF_printLog("[错误] " __VA_ARGS__), STF_printLog("\n")
#define OS_LOG(...) STF_printLog(__VA_ARGS__), STF_printLog("\n")

//校验宏。
#define OS_CHK(x) \
    {\
        if (!(x))\
        {\
            OS_LOG_ERR("["#x"]");\
            abort();\
        }\
    }

//带描述的校验宏。
#define OS_CHK_DESC(x, desc) \
    {\
        if (!(x))\
        {\
            OS_LOG_ERR(desc"["#x"]");\
            abort();\
        }\
    }

//函数返回值处理。
#define OS_CALL_FN(name, ulRslt) \
    {\
        if (RET_OK != ulRslt)\
        {\
            STF_printLog(""#name" failed!!! - 0x%x.", ulRslt);\
        }/*\
        else\
        {\
            OS_LOG_INFO(#name" ok.");\
        }*/\
    }

#define OS_CALL_FN_RET0(name, ulRslt) \
{\
    ALM_UINT64 _ulRet_maroc_tmp_ = (ulRslt);\
    OS_CALL_FN(name, _ulRet_maroc_tmp_);\
    if (RET_OK != _ulRet_maroc_tmp_)\
    {\
        return ;\
    }\
}

#define OS_CALL_FN_RET(name, ulRslt) \
{\
    ALM_UINT64 _ulRet_maroc_tmp_ = (ulRslt);\
    OS_CALL_FN(name, _ulRet_maroc_tmp_);\
    if (RET_OK != _ulRet_maroc_tmp_)\
    {\
        return RET_ERR;\
    }\
}

#define OS_CALL_FN_LAB(name, ulRslt, lab) \
{\
    ALM_UINT64 _ulRet_maroc_tmp_ = (ulRslt);\
    OS_CALL_FN(name, _ulRet_maroc_tmp_);\
    if (RET_OK != _ulRet_maroc_tmp_)\
    {\
        goto lab;\
    }\
}

#define OS_FN_RET(iRet) {if(RET_OK != (iRet)) return RET_ERR;}
#define OS_FN_RET0(iRet) {if(RET_OK != (iRet)) return ;}
#define OS_FN_LAB(iRet, lab) {if(RET_OK != (iRet)) goto lab;}

#endif // _os_h_
