#include <stdio.h>
#include <stdarg.h>
#include "DCL_Error.h"

static char szError[256];
static bool bError=false;

char* DCL_GetErrorStr()
{
	if(bError)
	{
		bError=false;
		return szError;
	}
	else return 0;
}

void DCL_SetErrorStr(char* szFormat,...)
{
	if(szFormat==0)
    	strcpy(szError,"CDCL_ERROR::SetErrorStr called with szFormat==NULL");
	else
	{
		va_list vl;
		va_start(vl,szFormat);
		vsnprintf(szError,256,szFormat,vl);
		va_end(vl);
	}
	bError=true;
}

bool DCL_IsError()
{
	return bError;
}