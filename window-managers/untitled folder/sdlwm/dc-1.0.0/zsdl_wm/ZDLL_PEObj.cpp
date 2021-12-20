#include <stdio.h>
#include <string.h>
#include "ZWM_Types.h"
#include "ZDLL_PEObj.h"

#ifndef WIN32
	#include <fcntl.h>
	#include <sys/mman.h>
	#include <unistd.h>
	#define strnicmp(a,b,n) strncasecmp(a,b,n)
#else
	#include <windows.h>
#endif

#include "DCL_MemMap.h"

typedef int(*FUNKPTR)();  

#define MakePtr( cast, ptr, addValue ) (cast)( (DWORD)(ptr) + (addValue) )

CZDLL_MODULE::CZDLL_MODULE():
	hMemMap(0),lpFileBase(0),bDllLoaded(false)
{

}

CZDLL_MODULE::~CZDLL_MODULE()
{
	UnloadWinDLL();
}

int CZDLL_MODULE::UnloadWinDLL()
{
	if(!bDllLoaded) return 0;
	
	llFunctions.DeleteList();

	DCL_CloseViewOfFile(hMemMap);
	return 0;
}

void* CZDLL_MODULE::GetSectionPtr(char* szName,void* lpVNTHeader,
								  DWORD dwImageBase)
{
	IMAGE_NT_HEADERS* lpNTHeader=(IMAGE_NT_HEADERS*)lpVNTHeader;
	IMAGE_SECTION_HEADER* lpSection;
	unsigned i;
	
	lpSection=(IMAGE_SECTION_HEADER*)(lpNTHeader+1);
	
	for(i=0;i<lpNTHeader->FileHeader.NumberOfSections;i++,lpSection++)
	{
		if(strnicmp((char*)lpSection->Name,szName,IMAGE_SIZEOF_SHORT_NAME)==0)
			return(void*)(lpSection->PointerToRawData+dwImageBase);
	}
	return 0;
}


int CZDLL_MODULE::Relocate(DWORD dwImageBase,DWORD dwMemPos,void* lpVNewHdr)
{
	long nDelta=dwMemPos-dwImageBase;
	FILE* file=fopen("c:\\reloc.txt","wt");
	IMAGE_NT_HEADERS32* lpNewHdr=(IMAGE_NT_HEADERS32*)lpVNewHdr;
	
	DWORD dwRel=lpNewHdr->OptionalHeader.DataDirectory
		[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
	IMAGE_BASE_RELOCATION* lpBaseReloc=(IMAGE_BASE_RELOCATION*)(dwRel+dwMemPos);

	fprintf(file,"base relocations:\n\n");

	while(lpBaseReloc->SizeOfBlock!=0)
	{	
		DWORD nEntries=(lpBaseReloc->SizeOfBlock-sizeof(*lpBaseReloc))/sizeof(WORD);
		WORD* lpEntry=MakePtr(WORD*,lpBaseReloc,sizeof(*lpBaseReloc) );
		
		fprintf(file,"Virtual Address: %08X  size: %08X\n",
				lpBaseReloc->VirtualAddress,lpBaseReloc->SizeOfBlock);
			
		for(DWORD i=0;i<nEntries;i++,lpEntry++)
		{
			WORD wRelType=(*lpEntry&0xF000)>>12;
			
			fprintf(file,"  %08X %i\n",
					(*lpEntry & 0x0FFF)+lpBaseReloc->VirtualAddress,wRelType);
			if(wRelType==3)
			{
				long* lpFix=(long*)(((*lpEntry&0x0FFF)+lpBaseReloc->VirtualAddress)+dwMemPos);
				(*lpFix)+=nDelta;
			}
		}
		lpBaseReloc=MakePtr(PIMAGE_BASE_RELOCATION,lpBaseReloc,
							 lpBaseReloc->SizeOfBlock);
	}
	fclose(file);
	return 0;
}

int CZDLL_MODULE::LoadWinDLL(char* szFile)
{
	hMemMap=DCL_MapViewOfFile(&lpFileBase,szFile,DCLMM_READ|DCLMM_WRITE|DCLMM_EXEC|DCLMM_WRITECOPY);
	
	IMAGE_DOS_HEADER*	lpDosHdr=(IMAGE_DOS_HEADER*)lpFileBase;
	if(lpDosHdr->e_magic!=IMAGE_DOS_SIGNATURE)
	{
		printf("No MZ header signature found\n");
		return -4;
	}
	IMAGE_NT_HEADERS32* lpNewHdr=(IMAGE_NT_HEADERS32*)((DWORD)(lpFileBase)+(lpDosHdr->e_lfanew));
	if(lpNewHdr->Signature!=IMAGE_NT_SIGNATURE)
	{
		printf("No PE Header signature found\n");
		return -5;
	}
	if(lpNewHdr->OptionalHeader.ImageBase!=(DWORD)lpFileBase)
	{
		printf("Base relocation needed ImageBase(0x%X)!=FileBase(0x%X)!\n",
			lpNewHdr->OptionalHeader.ImageBase,lpFileBase);
		if(Relocate(lpNewHdr->OptionalHeader.ImageBase,(DWORD)lpFileBase,
			lpNewHdr)<0) 
			return -6;
	}

	DWORD dwExpRVA=(DWORD)lpNewHdr->OptionalHeader.
			DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

	IMAGE_EXPORT_DIRECTORY* lpExpDir=(IMAGE_EXPORT_DIRECTORY*)(dwExpRVA+(DWORD)lpFileBase);

	char**	szFnkName=(char**)(lpExpDir->AddressOfNames+(DWORD)lpFileBase);
	DWORD**	lpFnkt=(DWORD**)(lpExpDir->AddressOfFunctions+(DWORD)lpFileBase);
	WORD*   lpOrd=(WORD*)(lpExpDir->AddressOfNameOrdinals+(DWORD)lpFileBase);

	for(unsigned int i=0;i<lpExpDir->NumberOfNames;i++)
	{
		CZDLL_FUNCTION* lpNewFn=new CZDLL_FUNCTION;
		lpNewFn->lpFuncPtr=(void*)(((DWORD)lpFnkt[0])+(DWORD)lpFileBase);
		lpNewFn->szFName=(*szFnkName+(DWORD)lpFileBase);
		lpNewFn->nOrdinal=*lpOrd+lpExpDir->Base;
		llFunctions.AddNode(lpNewFn);

		szFnkName++;
		lpFnkt++;
		lpOrd++;
	}

	bDllLoaded=true;
	return 0;
}

void* CZDLL_MODULE::GetFunction(int nOrd)
{
	CZDLL_FUNCTION* lpCurFn=(CZDLL_FUNCTION*)llFunctions.lpFirstNode;
	for(unsigned int i=0;i<llFunctions.nNumNodes;i++)
	{
		if(nOrd==lpCurFn->nOrdinal) return lpCurFn->lpFuncPtr;
		lpCurFn=(CZDLL_FUNCTION*)lpCurFn->lpNextNode;
	}
	return 0;
}

void* CZDLL_MODULE::GetFunction(char* szName)
{
	CZDLL_FUNCTION* lpCurFn=(CZDLL_FUNCTION*)llFunctions.lpFirstNode;
	for(unsigned int i=0;i<llFunctions.nNumNodes;i++)
	{
		if(strcmp(szName,lpCurFn->szFName)==0) return lpCurFn->lpFuncPtr;
		lpCurFn=(CZDLL_FUNCTION*)lpCurFn->lpNextNode;
	}
	return 0;
}

void* CZDLL_MODULE::GetFunctionLazy(char* szName)
{
	CZDLL_FUNCTION* lpCurFn=(CZDLL_FUNCTION*)llFunctions.lpFirstNode;
	for(unsigned int i=0;i<llFunctions.nNumNodes;i++)
	{
		if((strcmp(szName,lpCurFn->szFName)==0)||
			(strncmp(szName,lpCurFn->szFName,strlen(szName))==0)) return lpCurFn->lpFuncPtr;
		lpCurFn=(CZDLL_FUNCTION*)lpCurFn->lpNextNode;
	}
	return 0;
}
