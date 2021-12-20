#ifndef DCL_ERROR_H
#define DCL_ERROR_H

char* DCL_GetErrorStr();
void DCL_SetErrorStr(char* szFormat,...);
bool DCL_IsError();

#endif