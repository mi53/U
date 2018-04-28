/*Test Group Functions File*/


#include "Tst_Group.h"
#include "group_config.inc"

#ifdef __cplusplus
extern "C" {
#endif

void Test_InitRegister(void)
{
    char* pGroup = NULL;
    int i;
    int count;
    RegisterGroupSt* pstGroup;

    if (Case_TestTmp == g_pOnlyCase || Group_TestTmp == g_pOnlyCase)
    {
        pstGroup = InitFunArrayTemp;
        count = sizeof(InitFunArrayTemp) / sizeof(RegisterGroupSt);
    }
    else
    {
        pstGroup = InitFunArray;
        count = sizeof(InitFunArray) / sizeof(RegisterGroupSt);
    }

    for (i = 0; i < count; i++)
    {
        pGroup = (char*)STF_Reg_TestGrp(pstGroup[i].name, pstGroup[i].func);
        pstGroup[i].func(pGroup);
    }
}

#ifdef __cplusplus
}
#endif
