#pragma once
#include <excpt.h>

#define TRY_BLOCK(fail_val, body) \
{ \
    __try { \
        body \
    } \
    __except (EXCEPTION_EXECUTE_HANDLER) { \
        printf("Exception 0x%08X in %s\n", GetExceptionCode(), __func__); \
        return fail_val; \
    } \
}