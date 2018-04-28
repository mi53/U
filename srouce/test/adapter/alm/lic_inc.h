/***************************************************************************
* 本文件用于包装ALM头文件，并提供几个ALM相关的工具类函数。
***************************************************************************/

#ifndef _lic_inc_h_
#define _lic_inc_h_

#include "alm_intf.h"
#include "alm_debug.h"
#include "os.h"
#include "alm_tool_base.h"
#include "Tst_Framework.h"

/***************************************************************************
* 配置: 基于ALM实现一些工具类函数。
***************************************************************************/

/* 定义状态简写 */
#define TL_ALM_STATE_INVALID    "INVALID"
#define TL_ALM_STATE_LIC_VALID      "VALID"
#define TL_ALM_STATE_GRACE      "GRACE"
#define TL_ALM_STATE_DEFAULT    "DFLT"
#define TL_ALM_STATE_DEMO       "DEMO"
#define TL_ALM_STATE_NORMAL     "NORAML"
#define TL_ALM_STATE_EMERGENCY  "EMERGENCY"
#define TL_ALM_STATE_STICK      "STICK"

/* 将BBOM类型转换为字符串 */
CONST ALM_CHAR* Tl_AlmBbomTypeToStr(ALM_ENUM(ALM_LIC_BBOM_TYPE_ENUM) eType);

/* 将License状态转换为字符串 */
CONST ALM_CHAR* Tl_AlmLicState2Str(CONST ALM_STATE_LIC_STRU* pstLicState);

/* 将License文件类型转换为字符串 */
CONST ALM_CHAR* Tl_AlmLicType2Str(ALM_ENUM(ALM_LIC_TYPE_ENUM) eType);

/* 将产品状态转换为字符串 */
CONST ALM_CHAR* Tl_AlmPrdState2Str(ALM_ENUM(ALM_STATE_PRD_ENUM) eState);

/* 将License状态转换为字符串 */
CONST ALM_CHAR* Tl_AlmState2Str(ALM_ENUM(ALM_STATE_LIC_ENUM) eState);

/* 将原因转换为字符串 */
CONST ALM_CHAR* Tl_AlmRsnToStr(ALM_UINT64 ulRsn, OUT ALM_CHAR* pcRsn,
    ALM_UINT32 uiRsnSize);

/* 将告警原因转换为字符串 */
CONST ALM_CHAR* Tl_AlmAlarmRsnToStr(ALM_UINT64 ulRsn, OUT ALM_CHAR* pcRsn,
	ALM_UINT32 uiRsnSize);

/*将告警名称转为字符串*/
CONST ALM_CHAR* Tl_AlmAlarmName2Str(ALM_UINT32 uiAlarmName);

/*将告警类型转为字符串*/
CONST ALM_CHAR* Tl_AlmAlarmType2Str(ALM_UINT32 uiAlarmType);

/* 一个工具性质的调试函数，用于将分钟转换为天、时、分字符串 */
CONST ALM_CHAR* Tl_AlmMins2DayStr(ALM_UINT32 uiMins);

/* 将License状态字符串转换为状态 */
ALM_INT32 Tl_AlmStr2State(CONST ALM_CHAR* pcStr,
    ALM_ENUM(ALM_STATE_LIC_ENUM)* peState);

/* 将产品状态字符串转换为状态 */
ALM_INT32 Tl_AlmStr2PrdState(CONST ALM_CHAR* pcStr,
    ALM_ENUM(ALM_STATE_PRD_ENUM)* peState);



#endif // _lic_inc_h_
