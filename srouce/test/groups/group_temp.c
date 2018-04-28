/*******************************************************************************
Description    : 用于验证问题的临时测试用例。
History        :
<version> <date>     <Author> <Description>
*******************************************************************************/
#include "test_common.h"
#include "alm_alg_key.h"
#include "alm_lic_parser_common.h"
#include "alm_lic_stick.h"
#include "test_stru_lic.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */


/************************************************************************
* 验证问题用的临时测试用例。
************************************************************************/
    
ALM_VOID Case_TestTmp(ALM_VOID)
{
    void* pTest = OS_MemAlmAlloc(10);
    OS_MemAlmFree(pTest);
}


/* 用于验证问题的临时测试组 */
ALM_VOID Group_TestTmp(ALM_CHAR* pGroup)
{
    STF_REG_CASE(Case_TestTmp);
}


#ifdef __cplusplus
#if __cplusplus
}
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */
