#ifndef _ZWM_DEBUG_TXT
#define _ZWM_DEBUG_TXT

#include <stdio.h>

void ZWMDBG_Init();
void ZWMDBG_Exit();
void ZWMDBG_Flush();
void ZWMDBG_Print(char* szFmt,...);

#endif