#ifndef __util_H_wHrR17cd_lSne_H4Pa_sXcb_uLHluEtH2mNB__
#define __util_H_wHrR17cd_lSne_H4Pa_sXcb_uLHluEtH2mNB__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
//=============================================================================
//                Constant Definition
//=============================================================================

//=============================================================================
//                Macro Definition
//=============================================================================
#ifndef snprintf
    #define snprintf _snprintf
#endif

#ifndef _toStr
    #define _toStr(a)       #a
#endif

#ifndef _err
    #define _err(string, ...)               do{ printf(string, __VA_ARGS__);                    \
                                                printf(" -> %s[%d]\n", __FUNCTION__, __LINE__); \
                                                while(1);										\
                                            }while(0)
#endif

#ifndef _msg
    #define _msg(isEnable, string, ...)      ((void)((isEnable) ? printf(string, __VA_ARGS__) : 0))
#endif

#ifndef ARRAY_SIZE
    #define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#endif


//=============================================================================
//                Structure Definition
//=============================================================================

//=============================================================================
//                Global Data Definition
//=============================================================================

//=============================================================================
//                Private Function Definition
//=============================================================================

//=============================================================================
//                Public Function Definition
//=============================================================================

#ifdef __cplusplus
}
#endif

#endif
