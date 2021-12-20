#ifndef _DCL_MEMMAP_H
#define _DCL_MEMMAP_H

#define DCLMM_READ		1
#define DCLMM_WRITE		2
#define DCLMM_EXEC		4
#define DCLMM_WRITECOPY 8
#define DCLMM_SHARE		16

#ifdef WIN32
#include <windows.h>
#include "dcl_types.h"

struct _SDCL_MM_WIN32
{
	HANDLE 	hFile;
	HANDLE 	hFileMapping;
	void*	lpBaseAddr;
};

typedef _SDCL_MM_WIN32* HDCL_MM;
#else

struct _SDCL_MM_LINUX
{
	int		nFile;
	void*	lpBaseAddr;
	int		nSize;
};

typedef _SDCL_MM_LINUX* HDCL_MM;
#endif

HDCL_MM DCL_MapViewOfFile(void** lpData,char* szFile,int nFlags);
int DCL_CloseViewOfFile(HDCL_MM lpMM);

#endif
