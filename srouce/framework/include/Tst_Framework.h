/*******************************************************************************
  Description    :
  FileName       : Tst_Framework.h
  History        :
  <version> <date>     <Author> <Description>
  Initial file.
*******************************************************************************/

#ifndef __TST_FRAMEWORK_H__
#define __TST_FRAMEWORK_H__

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif/*ifdef __cplusplus */
#endif /*if __cplusplus*/

#include <stdio.h>
#include <stdlib.h>
#include "Tst_FrameWrkMacro.h"

#define TST_WIN32 0
#define TST_LINUX 1
#define TST_VXWORKS 2

#define TST_OS_WIN32 TST_WIN32
#define TST_OS_LINUX TST_LINUX
#define TST_OS_VXWORKS TST_VXWORKS

//#define TST_OS_VERSION TST_LINUX

#ifdef WIN32

//#include "windows.h"
 #define TST_OS_VERSION TST_OS_WIN32
#else
 #define TST_OS_VERSION TST_OS_LINUX
#endif

void X_LogWrite(char *pbuf);

#define LOG_FEATURE     STF_ENABLE
#define CONSOLE_LOG     STF_ENABLE

#define Frm_printf      printf
#define TST_WORKPATH    "../../../work/"
#define TOTAL_REPORT    TST_WORKPATH"report/TestReport.html"
#define SUMMARY_REPORT  TST_WORKPATH"report/TestReportSummary.html"
#define LOGFILE         TST_WORKPATH"report/log.txt"
#define CIFILE          TST_WORKPATH"report/CIfail.txt"
#define PS_FILE         TST_WORKPATH"persistence_files/ps.txt"

#define STF_REG_CASE(fnCase) STF_Reg_TestCase(__FUNCTION__, #fnCase, fnCase)
#define STF_ADD_GROUP(fnGroup) {fnGroup, #fnGroup}

typedef void (*TESTCASE_ENTRY_TYPE)( void);

typedef struct stTestFunctionList
{
    char                acTestFunctionName[256];
    TESTCASE_ENTRY_TYPE pTestFunc;
    struct stTestFunctionList * pstNext;
} stTestFunctionList;

typedef struct stTestGroupList
{
    char acTestGroupName[256];
    struct stTestFunctionList *pstTestcaseList;
    const void* pfnGroup;
} stTestGroupList;

const char* STF_Reg_TestGrp(const char* strName, const void* pfnGroup);
void STF_Reg_TestCase(const char* strGrpName, const char * strFuncName,
                             TESTCASE_ENTRY_TYPE pTestFunc);

int GetGrpIndex(const char* strGrpName);
void FRM_InsertGroup(const char* strName, const void* pfnGroup);
void FRM_InsertTestcase(stTestFunctionList *pstFuncList, int index);
void FRM_ClearTestcase();
int init_log();
void tstcleanup_log(void);
void TestResut(char *pstatus, int LineNo, char *pFileName);
void Test_Summary();
void ResetRunCount();
char* CommandLineExecution(char *pTestcaseName);
void STF_printLog(char *fmt, ...);
void SetVerdict(int verdict, int lineNo, char *pfilename);
void STF_Execute();

#ifdef __cplusplus
 #if __cplusplus
}
 #endif/*ifdef __cplusplus */
#endif /*if __cplusplus*/

#endif /* __TST_FRAMEWORK_H__*/
