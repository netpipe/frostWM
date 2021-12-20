#include <stdio.h>
#include <stdarg.h>
#include "ZWM_Debug.h"

void ZWMDBG_Init()
{
#ifdef _DEBUG_ZWM
	freopen("\\\\192.168.0.84\\debug\\stdout.txt","wt",stdout);
	freopen("\\\\192.168.0.84\\debug\\stderr.txt","wt",stderr);
#endif
}

void ZWMDBG_Print(char* szFmt,...)
{
#ifdef _DEBUG_ZWM
	va_list vl;
	FILE* file=fopen("\\\\192.168.0.84\\debug\\stddbg.txt","wt");
	if(!file) return;
	va_start(vl,szFmt);
	vfprintf(file,szFmt,vl);
	va_end(vl);
	fclose(file);
#endif
}

void ZWMDBG_Exit()
{
#ifdef _DEBUG_ZWM
	fclose(stdout);
	fclose(stderr);
#endif
}

void ZWMDBG_Flush()
{
#ifdef _DEBUG_ZWM
	fflush(stdout);
	fflush(stderr);
#endif
}