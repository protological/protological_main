#ifndef __DEFS_H__
#define __DEFS_H__

#include <stdint.h>
#include <stdbool.h>

#define strset(B,S) (strncpy((B),(S),sizeof(S)-1),(int)(sizeof(S)-1));

#define MSEC_PER_SEC    1000
#define USEC_PER_SEC    1000000

extern bool g_running;

#endif

// EOF
