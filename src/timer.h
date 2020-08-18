#ifndef _H_TIMER_
#define _H_TIMER_

#include "types.h"

#define PROFILER_START uint64 timer_start = GetTimeMs64()
#define PROFILER_END printf("Elapse time in ms: %lld\n", GetTimeMs64() - timer_start)

uint64 GetTimeMs64();

#endif