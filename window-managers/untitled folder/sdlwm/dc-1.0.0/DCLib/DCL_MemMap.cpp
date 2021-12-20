#include <string.h>
#include <stdio.h>

#ifdef WIN32

#else
	#include <unistd.h>
	#include <sys/mman.h>
	#include <fcntl.h>
#endif

#include "DCL_MemMap.h"

HDCL_MM DCL_MapViewOfFile(void** lpData,char* szFile,int nFlags)
{
	*lpData=0;
#ifdef WIN32

	int nCF1=GENERIC_READ; // GENERIC_READ
	int nCF2=FILE_SHARE_READ; // FILE_SHARE_READ
	int nCF3=OPEN_EXISTING; // OPEN_EXISTING
	int nCF4=FILE_ATTRIBUTE_NORMAL; // FILE_ATTRIBUTE_NORMAL

	int nCFM1=0; // PAGE_WRITECOPY

	if(nFlags&DCLMM_READ) nCFM1=PAGE_READONLY;
	if(nFlags&DCLMM_WRITE) 
	{
		nCFM1=PAGE_READWRITE;
		nCF1|=GENERIC_WRITE;
	}
	if(nFlags&DCLMM_WRITECOPY) 
	{
		nCFM1=PAGE_WRITECOPY;
		nCF1|=GENERIC_WRITE;
	}

	HDCL_MM hMM=new _SDCL_MM_WIN32;
	memset(hMM,0,sizeof(_SDCL_MM_WIN32));

	hMM->hFile=CreateFile(szFile,nCF1,nCF2,NULL,
                        nCF3,nCF4,0);
	hMM->hFileMapping=CreateFileMapping(hMM->hFile,NULL,nCFM1,0,0,NULL);
	hMM->lpBaseAddr=MapViewOfFile(hMM->hFileMapping,FILE_MAP_COPY,0,0,0);
	*lpData=hMM->lpBaseAddr;

#else
	
	int nOP1=0; // 0
	int nMM1=0; // PROT_EXEC|PROT_READ
	int nMM2=0; // MAP_SHARED

	if(nFlags&DCLMM_READ) nMM1|=PROT_READ;
	if(nFlags&DCLMM_WRITE) nMM1|=PROT_WRITE;
	if(nFlags&DCLMM_EXEC) nMM1|=PROT_EXEC;
	if(nFlags&DCLMM_WRITECOPY) nMM2|=MAP_PRIVATE;
	if(nFlags&DCLMM_SHARE) nMM2|=MAP_SHARED;

	if(nMM1==0) nMM1=PROT_NONE;

	HDCL_MM hMM=new _SDCL_MM_LINUX;
	memset(hMM,0,sizeof(_SDCL_MM_LINUX));
	
	FILE* file=fopen(szFile,"rb");
	fseek(file,0,SEEK_END);
	hMM->nSize=ftell(file);
	fclose(file);

	hMM->nFile=open(szFile,nOP1);
	hMM->lpBaseAddr=mmap(0,hMM->nSize,
		nMM1,nMM2,hMM->nFile,0);
	*lpData=hMM->lpBaseAddr;

#endif

	return hMM;
}

int DCL_CloseViewOfFile(HDCL_MM hMM)
{
#ifdef WIN32
	if(hMM->hFile!=INVALID_HANDLE_VALUE) 
		CloseHandle(hMM->hFile);
	if(hMM->hFileMapping!=INVALID_HANDLE_VALUE)
		CloseHandle(hMM->hFileMapping);
	UnmapViewOfFile(hMM->lpBaseAddr);

#else
	if(hMM->lpBaseAddr) munmap(hMM->lpBaseAddr,hMM->nSize);
#endif

	if(hMM) delete hMM;

	return 1;
}
