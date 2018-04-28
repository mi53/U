/*******************************************************************************
  Description    :
  FileName       : Tst_FrameWrkMacro.h
  History        :
  <version> <date>     <Author> <Description>
  Initial file.
*******************************************************************************/

#ifndef __TST_FRAMEWRKMACRO_H__
#define __TST_FRAMEWRKMACRO_H__

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif/*ifdef __cplusplus */
#endif /*if __cplusplus*/

#define NULL_PTR (0L)
#define FRM_ERR -1
#define FRM_SUCCESS 0
#define FRM_PASS 1
#define FRM_FAIL 0
#define FRM_INVALID 2

#define STF_ENABLE 1
#define STF_DISABLE 0

extern unsigned int g_TotalFailedAssert;

#define _STF_ASSERT_FAIL_OUT() \
    STF_printLog("[ERROR] assert fail in \" %s - %d\"\n", \
    __FUNCTION__, __LINE__); g_TotalFailedAssert++; g_bLastAssertRslt = 0;


extern unsigned int g_bLastAssertRslt;
#define STF_ASSERT_LAST_RSLT() (!!g_bLastAssertRslt)

#define _STF_ASSERT_DO(value, doing) \
    do  { \
        if (!(value) || !STF_ASSERT_LAST_RSLT()) \
        { \
            _STF_ASSERT_FAIL_OUT(); \
            doing;\
        } \
    } while (0);

#define STF_ASSERT(value) _STF_ASSERT_DO(value, (void)0);
#define STF_ASSERT_VOID(value) _STF_ASSERT_DO(value, return)
#define STF_ASSERT_RET(value, ret) _STF_ASSERT_DO(value, return ret)
#define STF_ASSERT_LAB(value, lab) _STF_ASSERT_DO(value, goto lab)

#define STF_ASSERT_EQUAL(actual, expected) \
    STF_ASSERT((actual) == (expected)) 

#define STF_ASSERT_EQUAL_FATAL(actual, expected) \
    STF_ASSERT_VOID((actual) == (expected)) 

#define STF_ASSERT_STRING_EQUAL(actual, expected) \
    STF_ASSERT(0 == strcmp(actual, expected)) 

#define STF_ASSERT_STRING_EQUAL_FATAL(actual, expected) \
    STF_ASSERT_VOID(0 == strcmp(actual, expected)) 

#define STF_ASSERT_PTR_NOT_NULL_FATAL(value) \
    STF_ASSERT_VOID(NULL != (value)) 

#ifdef __cplusplus
 #if __cplusplus
}
 #endif/*ifdef __cplusplus */
#endif /*if __cplusplus*/

#endif /* __TST_FRAMEWRKMACRO_H__*/
