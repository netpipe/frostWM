#include <stdio.h>
#include "DCLib.h"
#include "DCL_Error.h"

int main(int argc,char* argv[])
{
	// Memmory Mapping !
	printf("\nMemmory Mapping class test : ");
	void* lpData;
	HDCL_MM hMap=DCL_MapViewOfFile(&lpData,"test_mmap.txt",DCLMM_READ|DCLMM_SHARE);

	char* lpChar=(char*)lpData;
	while(lpChar[0]!='\n')
	{
		putchar(lpChar[0]);
		lpChar++;
	}
	putchar('\n');

	DCL_CloseViewOfFile(hMap);

	// IniFileParsing !
	printf("\nIniFileParser class test : ");
	CDCL_INIFILE cIni;
	int nRet=cIni.Load("win.ini",DCLINI_NONCASESENSITIV);
	printf("\"%i\" -",nRet);
	if(nRet)
	{
		printf(" Success\n");
	}
	else
	{
		char* szErr=DCL_GetErrorStr();
		printf(" Error: %s\n",szErr);
	}
	
	HDCL_INISEC hSec=cIni.GetSection("WS_FTP");
	if(hSec)
	{
		char* szPath=cIni.GetStringValue(hSec,"DIR");
		if(szPath)
			printf("Section [WS_FTP] Entry DIR = (%s)\n",szPath);
		else printf("Entry [DIR] in section [WS_FTP] not found !\n");
		
		int nInt=cIni.GetIntValue(hSec,"INSTOPTS");
		printf("Section [WS_FTP] Entry INSTOPTS = (%i)\n",nInt);
	}
	else printf("Section [WS_FTP] not found !\n");
	return 1;
}