/*******************************************************************************
Description    : 
History        :
<version> <date>     <Author> <Description>
*******************************************************************************/
#include "Tst_Group.h"
#include "callback.h"
#include "alm_intf.h"
#include "alm_state_mgr.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */

ALM_VOID Test_Start();

ALM_VOID Test_Stop();

/* 完成ALM启动过程 */
ALM_INT32 Test_Init(ALM_BOOL bCLsPs, ALM_BOOL bFixHour, ALM_UINT8 ucFlashHour,
    ALM_UINT8 ucFlashInterval);
ALM_VOID Test_Cls();
ALM_INT32 Test_AddPrd(ALM_INT32 iPrdId);

ALM_VOID PrintAdditon(ALM_LIC_STORE_ADDITION_STRU *pstAddtion);
ALM_VOID PrintGener(ALM_LIC_STORE_GENERAL_STRU *pstGener);
ALM_VOID PrintLicOffers(ALM_LIC_STORE_OFFERS_STRU *stOffersQuerryRslt);
ALM_VOID PrintLicBboms(ALM_LIC_STORE_BBOMS_STRU *pstBboms);


//读取文件到缓存中，需要外部释放缓存內存。
int Test_ReadFileToBuf(const char* pcFile, OUT char** ppcBuf, OUT int* piReadLen);

//内存释放函数，仅在参数不为ALM_NULL时释放。并置为ALM_NULL。
void Test_FreePP(void** ppMem);
#define TEST_FREE(ppMem) Test_FreePP((void**)(ppMem))

#define TEST_KEY g_pcKey
extern char* g_pcKey;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */
