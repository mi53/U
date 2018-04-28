/* 本文件中添加新增group */
#include "Tst_Group.h"
#include "group_store_tree.h"
#include "group_prd_mgr.h"
#include "group_lic_store.h"
#include "group_prd_store.h"
#include "group_lic_stick.h"
#include "group_lic_em.h"
#include "group_lic_ger.h"
#include "group_persistence_wr.h"
#include "group_state_mgr.h"
#include "group_key_alg.h"
#include "group_lic_parser_v2.h"
#include "group_lic_parser_v3.h"
#include "group_any_demo.h"
#include "group_alarm.h"
#include "group_safetime.h"
#include "group_emstarttimes.h"
#include "group_per_temp_bbommin.h"
#include "group_disable.h"
#include "group_ps_store_lic_and_prd.h"
#include "group_lic_esn.h"

#ifdef __cplusplus
extern "C" {
#endif


/* 用于验证问题的临时测试组 */
ALM_VOID Group_TestTmp(ALM_CHAR* pGroup);

//用于定位问题的临时测试用例.
void Case_TestTmp(void);

//若为ALM_NULL运行全部非定位问题的临时测试组。否则只运行指定的用例或测试组。
const void* g_pOnlyCase = Group_StoreTree; //Group_AnyDemoLic

/* 自此添加测试组 */
RegisterGroupSt InitFunArray[] =
{
    STF_ADD_GROUP(Group_KeyAndAlg),
    STF_ADD_GROUP(Group_StoreTree),
	STF_ADD_GROUP(Group_PSWR),
	STF_ADD_GROUP(Group_PrdMgr),
	STF_ADD_GROUP(Group_LicStore),
	STF_ADD_GROUP(Group_PrdStore),
    STF_ADD_GROUP(Group_LicStick),
    STF_ADD_GROUP(Group_LicEm),
    STF_ADD_GROUP(Group_LicGer),
    STF_ADD_GROUP(Group_StateMgr),
	STF_ADD_GROUP(Group_LicParserV2),
	STF_ADD_GROUP(Group_LicParserV3),
	STF_ADD_GROUP(Group_AnyDemoLic),
	STF_ADD_GROUP(Group_Alarm),
	STF_ADD_GROUP(Group_SafeTime),
	STF_ADD_GROUP(Group_LicEmTimes),
	STF_ADD_GROUP(Group_LicBBOMMinValue),
	STF_ADD_GROUP(Group_Disable),
	STF_ADD_GROUP(Group_PSStore),
//	STF_ADD_GROUP(Group_LicEsn),
};

/* 临时用于定位问题的用例组 */
RegisterGroupSt InitFunArrayTemp[] =
{
    STF_ADD_GROUP(Group_TestTmp),
};



#ifdef __cplusplus
}
#endif
