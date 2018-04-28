/*******************************************************************************
Description    : 
History        :
<version> <date>     <Author> <Description>
*******************************************************************************/
#ifndef __TST_GROUP_H__
#define __TST_GROUP_H__

#include "Tst_Framework.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    void (*func)(char* pGroup);
    char name[100];
} RegisterGroupSt;

void Test_InitRegister(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */

#endif /* __TST_GROUP_H__ */
