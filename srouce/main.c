#include "test_common.h"
#include "Tst_Group.h"
#include "alm_intf.h"
#include <time.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif/*ifdef __cplusplus */
#endif /*if __cplusplus*/

void test_heap_mem_check_is_start()
{
    char* a = (char*)malloc(1);
    int i;

    for (i = 0; i < 100; ++i)
    {
        a[i] = i;
    }

    STF_printLog("Not start heap memory check.\n");
}

int main()
{
    //test_heap_mem_check_is_start();

    Test_Start();

    STF_Execute();

    Test_Summary();

    Test_Stop();

    system("pause");

    return 0;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif/*ifdef __cplusplus */
#endif /*if __cplusplus*/
