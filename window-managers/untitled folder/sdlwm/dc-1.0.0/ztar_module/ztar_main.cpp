#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ztar_module.h"
#include "ZWM_Debug.h"
#include "ZSDL_WM.h"

#include <SDL_image.h>

char* szLnkFlags[]=
{
	"Normal",
	"Link",
	"SymbolicLink",
	"CharacterDevice",
	"BlockDevice",
	"Directory",
	"FIFO File",
	"Contiguous File",
	"EOF"
};

FILE* ZOpenTarFile(char* szTarFile,char* szFile)
{
	char szBuffer[256];
	strcpy(szBuffer,CZSDL_WM::szWM_HDir);
	strcat(szBuffer,szTarFile);

	FILE* file=fopen(szBuffer,"rb");
	if(file==0)
	{
		file=fopen(szTarFile,"rb");
		if(file==0) return 0;
	}
	
	ZTAR_RECORD tarRecord;

	tarRecord.tarHeader.linkflag='0';
	do
	{
		fread(&tarRecord,ZTAR_RECORDSIZE,1,file);

		long lSize=strtoul(tarRecord.tarHeader.size,0,8);
		long nBlocks=(lSize%512)?(lSize/512)+1:lSize/512;
		if(strcmp(szFile,tarRecord.tarHeader.szName)==0)
		{
			return file;
		}
		fseek(file,nBlocks*512,SEEK_CUR);
	}while(tarRecord.tarHeader.linkflag!=0);
	fclose(file);
	return 0;
} 

SDL_Surface* ZLoadTarImage(char* szTarFile,char* szFile)
{
	FILE* file=ZOpenTarFile(szTarFile,szFile);
	if(!file) return 0;
	long nOffset=ftell(file);
	fclose(file);

	char szBuffer[256];
	strcpy(szBuffer,CZSDL_WM::szWM_HDir);
	strcat(szBuffer,szTarFile);
	SDL_RWops *src=SDL_RWFromFile(szBuffer,"rb");
	if(src==0)
	{
		src=SDL_RWFromFile(szTarFile,"rb");
		if(src==0) return 0;
	}
	
	SDL_RWseek(src,nOffset,SEEK_SET);
	SDL_Surface* lpSrf=IMG_Load_RW(src,1);
	
	return lpSrf;
}

void ZLoadTarDirToList(CZWM_LLIST* lpList,char* szTar,char* szPath)
{
	FILE* file=fopen(szTar,"rb");
	ZTAR_RECORD tarRecord;

	tarRecord.tarHeader.linkflag='0';

	do
	{
		fread(&tarRecord,ZTAR_RECORDSIZE,1,file);
		//if(tarRecord.tarHeader.linkflag==ZTAR_LF_DIR) break;

		long lSize=strtoul(tarRecord.tarHeader.size,0,8);
		long nBlocks=(lSize%512)?(lSize/512)+1:lSize/512;
		
		CZWM_FILENODE* lpNewFN=new CZWM_FILENODE;
		strcpy(lpNewFN->szText,tarRecord.tarHeader.szName);
		lpNewFN->nType=ZFL_FILE;

		lpList->AddNode(lpNewFN);
		
		fseek(file,nBlocks*512,SEEK_CUR);
	}
	while(tarRecord.tarHeader.linkflag!=0&&!feof(file));
	
	fclose(file);
}

void ZLoadTarToList(CZWM_LLIST* lpList,char* szTar,char* szPath)
{
	FILE* file=fopen(szTar,"rb");
	ZTAR_RECORD tarRecord;

	tarRecord.tarHeader.linkflag='0';

	do
	{
		fread(&tarRecord,ZTAR_RECORDSIZE,1,file);
		//if(tarRecord.tarHeader.linkflag==ZTAR_LF_DIR) break;

		long lSize=strtoul(tarRecord.tarHeader.size,0,8);
		long nBlocks=(lSize%512)?(lSize/512)+1:lSize/512;
		
		CZWM_FILENODE* lpNewFN=new CZWM_FILENODE;
		strcpy(lpNewFN->szText,tarRecord.tarHeader.szName);
		lpNewFN->nType=ZFL_FILE;

		lpList->AddNode(lpNewFN);
		
		fseek(file,nBlocks*512,SEEK_CUR);
	}
	while(tarRecord.tarHeader.linkflag!=0&&!feof(file));
	
	fclose(file);
}