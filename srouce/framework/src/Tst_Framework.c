/*******************************************************************************
  Description    :
  FileName       : Tst_Framework.c
  History        :
  <version> <date>     <Author> <Description>
  Initial file.
*******************************************************************************/
#include "Tst_Framework.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>


stTestGroupList g_aGroupTbl[100];
unsigned int g_GrpTblindex = 0;
unsigned int g_TotalTestCaseCount = 0;
unsigned int g_TotalTestCaseRun = 0;
unsigned int g_TotalPassed = 0;
unsigned int g_TotalFailed = 0;
unsigned int g_TotalFailedAssert = 0;
unsigned int g_bLastAssertRslt = 0;

char g_aucGroupName[100];
char g_aucTestcaseName[100];
FILE *fphtml = NULL;
FILE *fpLog = NULL;
FILE *fpfailCI = NULL;
unsigned int g_consolePrint = CONSOLE_LOG;
extern const void* g_pOnlyCase;

const char* STF_Reg_TestGrp(const char* strName, const void* pfnGroup)
{
    FRM_InsertGroup(strName, pfnGroup);
    return strName;
}

void STF_Reg_TestCase(const char* strGrpName, const char * strFuncName,
                      TESTCASE_ENTRY_TYPE pTestFunc)
{
    int index = 0;
    stTestFunctionList *pstFuncList;

    pstFuncList = (stTestFunctionList *)malloc(sizeof(stTestFunctionList));
    if (pstFuncList == NULL_PTR)
    {
        STF_printLog("\n Test case addition in the suite failed due to malloc failure\n");
        return;
    }

    index = GetGrpIndex(strGrpName);
    if (index == FRM_ERR)
    {
        STF_printLog("\n The Group is not added for the Test Case");
        free(pstFuncList);
        return;
    }

    strcpy(pstFuncList->acTestFunctionName, strFuncName);
    pstFuncList->pTestFunc = pTestFunc;
    pstFuncList->pstNext = NULL_PTR;

    FRM_InsertTestcase(pstFuncList, index);
    g_TotalTestCaseCount++;
}

int GetGrpIndex(const char* strGrpName)
{
    unsigned int i = 0;
    int retVal = 0;

    for (i = 0; i <= g_GrpTblindex; i++)
    {
        retVal = strcmp(strGrpName, g_aGroupTbl[i].acTestGroupName);
        if (retVal == 0)
        {
            return i;
        }
    }

    return FRM_ERR;
}

void FRM_InsertGroup(const char* strName, const void* pfnGroup)
{
    if (g_GrpTblindex < 100)
    {
        strcpy(g_aGroupTbl[g_GrpTblindex].acTestGroupName, strName);
        g_aGroupTbl[g_GrpTblindex].pstTestcaseList = NULL_PTR;
        g_aGroupTbl[g_GrpTblindex].pfnGroup = pfnGroup;
        g_GrpTblindex++;
    }
    else
    {
        STF_printLog("\n The group Addition reached the maximum \n");
        STF_printLog("\n Change the Group Maximum limit \n");
    }
}

void FRM_InsertTestcase(stTestFunctionList *pstFuncList, int index )
{
    stTestFunctionList *pstempList = NULL_PTR;

    if (g_aGroupTbl[index].pstTestcaseList == NULL_PTR)
    {
        g_aGroupTbl[index].pstTestcaseList = pstFuncList;
        return;
    }

    pstempList = g_aGroupTbl[index].pstTestcaseList;

    while (pstempList->pstNext)
    {
        pstempList = pstempList->pstNext;
    }

    pstempList->pstNext = pstFuncList;
}

void FRM_ClearTestcase()
{
    stTestFunctionList *pstempList = NULL_PTR;
    stTestFunctionList *pstempDel = NULL_PTR;
    unsigned int idx;

    for (idx = 0; idx < g_GrpTblindex; ++idx) {
        pstempList = g_aGroupTbl[idx].pstTestcaseList;
        while (pstempList != NULL)
        {
            pstempDel = pstempList;
            pstempList = pstempList->pstNext;

            free(pstempDel);
        }
        g_aGroupTbl[idx].pstTestcaseList = NULL;
    }
}

void ResetRunCount()
{
    g_TotalTestCaseRun = 0;
    g_TotalPassed = 0;
    g_TotalFailed = 0;
    g_TotalFailedAssert = 0;
}

void WriteGropuTableToHtml(char *aucGroupName)
{
    static int iCount = 0;
    char *acHead =\
        "<center><table style=\"border-collapse: collapse\" cellSpacing=\"0\"" \
        "cellPadding=\"0\" width=\"766\" border=\"1\" bordercolor=\"#CCCCCC\" height=\"63\">" \
        "<tr><td width=\"1063\" bgcolor=\"#E5E5E5\" colspan=\"4\" bordercolor=\"#000000\" height=\"15\">" \
        "<p align=\"center\"><font face=\"Lucida Bright\" size=\"2\"><b>";

    char *acTail =\
        "</b></font></td></tr> <tr bgColor=\"lightGrey\"> <td width=\"162\"" \
        "bgcolor=\"#3399FF\" bordercolor=\"#000000\" height=\"15\"><b>" \
        "<font face=\"Lucida Bright\" size=\"2\">SR NO</font></b></td>" \
        "<td width=\"424\" bgcolor=\"#3399FF\" bordercolor=\"#000000\" height=\"15\"><b>" \
        "<font face=\"Lucida Bright\" size=\"2\">TEST CASE </font></b></td>" \
        "<td width=\"207\" bgcolor=\"#3399FF\" bordercolor=\"#000000\" height=\"15\"><b>" \
        "<font face=\"Lucida Bright\" size=\"2\">RESULT</font></b></td>" \
        "<td width=\"270\" bgcolor=\"#3399FF\" bordercolor=\"#000000\" height=\"15\"><b>" \
        "<font face=\"Lucida Bright\" size=\"2\">REMARKS</font></b></td></tr>";

    if (iCount)
    {
        fputs("</table></center>", fphtml);
        fputs("<p align=\"center\"></p>", fphtml);
    }

    fputs(acHead, fphtml);

    /* Total testcase */
    fputs(aucGroupName, fphtml);
    fputs(acTail, fphtml);
    fflush(fphtml);

    iCount++;
}

void STF_Execute()
{
    unsigned int i = 0;
    stTestFunctionList *pstempList = NULL_PTR;
    char bWriteGroup;

    ResetRunCount();
    for (i = 0; i < g_GrpTblindex; i++)
    {
        bWriteGroup = 0;
        pstempList = g_aGroupTbl[i].pstTestcaseList;
        strcpy(g_aucGroupName, g_aGroupTbl[i].acTestGroupName);
        while (pstempList)
        {
            g_TotalFailedAssert = 0;
            if (pstempList->pTestFunc != NULL
                && (NULL == g_pOnlyCase || pstempList->pTestFunc == g_pOnlyCase
                || g_aGroupTbl[i].pfnGroup == g_pOnlyCase))
            {
                if (!bWriteGroup)
                {
                    STF_printLog(
                        "*****************************************************\n");
                    STF_printLog("GROUP: %s\n", g_aucGroupName);
                    STF_printLog(
                        "*****************************************************\n");
                    WriteGropuTableToHtml(g_aucGroupName);
                    bWriteGroup = 1;
                }

                STF_printLog("================================\n");
                STF_printLog("CASE: %s\n", pstempList->acTestFunctionName);
                STF_printLog("================================\n");

                g_bLastAssertRslt = 1;

                pstempList->pTestFunc();

                g_TotalTestCaseRun++;

                strncpy(g_aucTestcaseName, pstempList->acTestFunctionName,
                    sizeof(g_aucTestcaseName));
                g_aucTestcaseName[sizeof(g_aucTestcaseName) - 1] = '\0';

                STF_printLog("- - - - - - - -\n");
                if (0 == g_TotalFailedAssert)
                {
                    TestResut("PASSED", 0, NULL);
                    STF_printLog("RSLT: PASSED\n");
                    g_TotalPassed++;
                }
                else
                {
                    TestResut("FAILED", 0, g_aucTestcaseName);
                    STF_printLog("RSLT: FAILED\n");
                }
                STF_printLog("- - - - - - - -\n");
            }

            pstempList = pstempList->pstNext;
        }
    }
}


char *summaryhtml = "<table border=\"1\" cellspacing=\"1\" width=\"78%\">" \
                    "<tr> <td width=\"75%\" bgcolor=\"#66CCFF\" colspan=\"4\">" \
                    "<p align=\"center\"><b><span style=\"background-color: #66CCFF\">" \
                    "SUMMARY REPORT</span></b><span style=\"background-color: #66CCFF\">" \
                    "</span></td>" \
                    "</tr>  <tr>" \
                    "<td width=\"17%\" bgcolor=\"#FFCCFF\"><b>Total Test Cases</b></td>" \
                    "<td width=\"15%\" bgcolor=\"#FFCCFF\"><b>&nbsp;Total Run</b></td>" \
                    "<td width=\"18%\" bgcolor=\"#FFCCFF\"><b>&nbsp;Success Cases </b></td>" \
                    "<td width=\"25%\" bgcolor=\"#FFCCFF\"><b>&nbsp;Failure Cases</b></td>" \
                    "</tr> <tr>";

int WriteSummaryHtml(FILE * fpsumhtml, char *filename, int iTotal, int iRun,
                     int iPassed,
                     int iFailed)
{
    char *genPass = "<td width=\"17%\" bgcolor=\"#99FF99\">&nbsp;<b>";
    char *genFail = "<td width=\"17%\" bgcolor=\"#FF0000\">&nbsp;<b>";
    char buf[100];

    fputs("</table><p></p>", fpsumhtml);
    fputs(summaryhtml, fpsumhtml);

    /* Total testcase */
    fputs(genPass, fpsumhtml);
    sprintf(buf, "%u", iTotal);
    fputs(buf, fpsumhtml);
    fputs("</b></td>", fpsumhtml);

    /* Total Run value */
    if (iTotal == iRun)
    {
        fputs(genPass, fpsumhtml);
        sprintf(buf, "%u", iRun);
        fputs(buf, fpsumhtml);
        fputs("</b></td>", fpsumhtml);
    }
    else
    {
        fputs(genFail, fpsumhtml);
        sprintf(buf, "%u", iRun);
        fputs(buf, fpsumhtml);
        fputs("</b></td>", fpsumhtml);
    }

    /* Total Passed value */
    fputs(genPass, fpsumhtml);
    sprintf(buf, "%u", iPassed);
    fputs(buf, fpsumhtml);
    fputs("</b></td>", fpsumhtml);

    /* Total Failed value */
    if (iFailed)
    {
        fputs(genFail, fpsumhtml);
        sprintf(buf, "%u", iFailed);
        fputs(buf, fpsumhtml);
        fputs("</b></td>", fpsumhtml);
    }
    else
    {
        fputs(genPass, fpsumhtml);
        sprintf(buf, "%u", iFailed);
        fputs(buf, fpsumhtml);
        fputs("</b></td>", fpsumhtml);
    }

    fputs("</tr></table><p align=\"center\"></p>", fpsumhtml);
    fflush(fpsumhtml);

    return 0;
}

int WriteSummaryHtml_Ex(char *filename, int iTotal, int iRun, int iPassed,
                        int iFailed)
{
    FILE *fpsumhtml = NULL;

    (void)WriteSummaryHtml(fphtml, filename, iTotal, iRun, iPassed, iFailed);
    fpsumhtml = fopen(filename, "w");
    if (NULL != fpsumhtml)
    {
        fputs("<html> <body>", fpsumhtml);
        (void)WriteSummaryHtml(fpsumhtml, filename, iTotal, iRun, iPassed,
                               iFailed);
        fputs("</body></html>", fpsumhtml);
        fflush(fpsumhtml);
        fclose(fpsumhtml);
    }

    return 0;
}

int init_log()
{
    char *acHead =\
        "<html><body><p align=\"center\"><b><font size=\"5\" color=\"#800080\">&nbsp;" \
        "<u>TEST REPORT</u></font></b>&nbsp;&nbsp;&nbsp;&nbsp;</p>";

    fphtml = fopen(TOTAL_REPORT, "w");
    if (NULL == fphtml)
    {
        Frm_printf("\n cann't open html file\n");
        return FRM_ERR;
    }

    if (STF_ENABLE == LOG_FEATURE)
    {
        fpLog = fopen(LOGFILE, "w");
        if (NULL == fphtml)
        {
            Frm_printf("\n cann't open file Log file\n");
            return FRM_ERR;
        }
    }

    fputs(acHead, fphtml);
    fflush(fphtml);

    return FRM_SUCCESS;
}

void X_LogWrite(char *pbuf)
{
    if ((pbuf != NULL) && (fpLog != NULL))
    {
        fwrite(pbuf, strlen(pbuf), 1, fpLog);
    }
}

void WritePassResultToHtml(char * aucTestcaseName)
{
    char buf[256] = {0};
    char *acHead = "<tr bgColor=\"lightBlue\">" \
                   "<td width=\"162\" bgcolor=lightgreen height=\"16\"><b><font face=\"System\">";

    char *acMid = "</font></b></td>" \
                  "<td width=\"424\" bgcolor=lightgreen height=\"16\"><b><font face=\"System\">";
    char *acTail = "</font></b></td>" \
                   "<td width=\"207\" bgcolor=lightgreen height=\"16\"><b><font face=\"System\">PASSED</font></b></td>" \
                   "<td width=\"270\" bgcolor=lightgreen height=\"16\"></td></tr>";

    fputs(acHead, fphtml);
    sprintf(buf, "%u", g_TotalTestCaseRun);
    fputs(buf, fphtml);
    fputs(acMid, fphtml);
    fputs(aucTestcaseName, fphtml);
    fputs(acTail, fphtml);
    fflush(fphtml);
}

void WriteFailResultToHtml(char * aucTestcaseName, char *acFileName,
                           int iLineNo)
{
    char *acHead = "<tr bgColor=\"lightBlue\">" \
                   "<td width=\"162\" bgcolor=\"#FF3300\" height=\"16\"><b><font face=\"Estrangelo Edessa\">";

    char *acMid1 = "</font></b></td>" \
                   "<td width=\"424\" bgcolor=\"#FF3300\" height=\"16\"><b><font face=\"Estrangelo Edessa\">";

    char *acMid2 = "</font></b></td>" \
                   "<td width=\"207\" bgcolor=\"#FF3300\" height=\"16\"><b><font face=\"Estrangelo Edessa\">FAILED</font></b></td>" \
                   "<td width=\"270\" bgcolor=\"#FF3300\" height=\"14\"><b>"
                   "<font face=\"Estrangelo Edessa\">";

    char buf[256] = {0};

    fputs(acHead, fphtml);
    sprintf(buf, "%u", g_TotalTestCaseRun);
    fputs(buf, fphtml);
    fputs(acMid1, fphtml);
    fputs(aucTestcaseName, fphtml);
    fputs(acMid2, fphtml);
    fputs(acFileName, fphtml);
    sprintf(buf, ":%u", iLineNo);
    fputs(buf, fphtml);
    fflush(fphtml);
    if (fpfailCI == NULL)
    {
        fpfailCI = fopen(CIFILE, "w");
        if (NULL == fpfailCI)
        {
            Frm_printf("\n cann't create the CIfail file \n");
            return;
        }
    }
}

void tstcleanup_log(void)
{
    char buf[50] = {0};

    if (fphtml != NULL)
    {
        strcpy(buf, "</TABLE></CENTER>");
        fputs(buf, fphtml);

        strcpy(buf, "</BODY></HTML>");
        fputs(buf, fphtml);

        fflush(fphtml);
        fclose(fphtml);
    }

    if (fpLog != NULL)
    {
        fclose(fpLog);
    }

    if (fpfailCI != NULL)
    {
        fclose(fpfailCI);
    }
}

void TestResut(char *pstatus, int LineNo, char *pFileName)
{
    if (!strcmp("PASSED", pstatus))
    {
        WritePassResultToHtml(g_aucTestcaseName);
    }
    else
    {
        WriteFailResultToHtml(g_aucTestcaseName, pFileName, LineNo);
    }

    return;
}


void SetVerdict(int verdict, int lineNo, char *pfilename)
{
    if (verdict == FRM_PASS)
    {
        //WritePassResultToHtml(g_aucTestcaseName);
    }
    else
    {
        g_TotalFailedAssert++;
        WriteFailResultToHtml(g_aucTestcaseName, pfilename, lineNo);
    }
}

void Test_Summary()
{
    STF_printLog("\n******************************************************\n");
    STF_printLog("Total Groups           = %u\n", g_GrpTblindex);
    STF_printLog("Total Testcase         = %u\n", g_TotalTestCaseCount);
    STF_printLog("Total Testcase Run     = %u\n", g_TotalTestCaseRun);
    STF_printLog("Total Test case Passed = %u\n", g_TotalPassed);
    (void)WriteSummaryHtml_Ex(SUMMARY_REPORT, g_TotalTestCaseCount,
                              g_TotalTestCaseRun, g_TotalPassed,
                              g_TotalTestCaseRun - g_TotalPassed);
    STF_printLog("******************************************************\n");
}

void Console_print(char *pbuffer)
{
    if (g_consolePrint && (pbuffer != NULL))
    {
        Frm_printf(pbuffer);
    }
}

void STF_printLog(char *fmt, ...)
{
    va_list ap;
    char* pcBuf = NULL;

    pcBuf = (char*)malloc(1024 * 10);
    va_start(ap, fmt);

    vsprintf(pcBuf, fmt, ap);
    Console_print(pcBuf);
    X_LogWrite(pcBuf);

    va_end(ap);
    free(pcBuf);
}

char* DecodeMessage(char* buffer)
{
    char *pstr = 0;

    if (0 == buffer)
    {
        return "SOMETHING WRONG IN MESSAGE RECEIVED";
    }

    pstr = strstr(buffer, ":");

    if (0 == pstr)
    {
        return "";
    }

    return pstr + 1;
}

char* GetReportPath(char* pcPath)
{
    static char acRepPath[1024];
    char *pcRepPath;

    pcRepPath = getenv("REP_PATH");
    if (NULL == pcRepPath)
    {
        STF_printLog("\n Error environment variable REP_PATH not set");
        return ("error");
    }
    else
    {
        sprintf(acRepPath, "%s/%s", pcRepPath, pcPath);
        return acRepPath;
    }
}
