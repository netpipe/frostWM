#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DCL_InitFileParser.h"
#include "DCL_Error.h"

CDCL_INIFILE::CDCL_INIFILE():lpBackupHook(0) {}
CDCL_INIFILE::~CDCL_INIFILE() {}

CDCL_INIFILE_ENTRY_NODE::CDCL_INIFILE_ENTRY_NODE():
	szEntryName(0),szEntryValue(0) {}
CDCL_INIFILE_ENTRY_NODE::~CDCL_INIFILE_ENTRY_NODE()
{
	if(szEntryName) delete[] szEntryName;
	if(szEntryValue) delete[] szEntryValue;
}

CDCL_INIFILE_SECTION_NODE::CDCL_INIFILE_SECTION_NODE():
	szSectionName(0) {}
CDCL_INIFILE_SECTION_NODE::~CDCL_INIFILE_SECTION_NODE()
{
	if(szSectionName) delete[] szSectionName;
	llEntries.Free();
}

HDCL_INISEC CDCL_INIFILE::GetSection(char* szName)
{
	unsigned int nNumNodes=llSections.GetNumNodes();
	HDCL_INISEC hCurSec=(HDCL_INISEC)llSections.GetFirstNode();
	for(unsigned int i=0;i<nNumNodes;i++)
	{
		if(strcmp(hCurSec->szSectionName,szName)==0) return hCurSec;
	 	hCurSec=(HDCL_INISEC)hCurSec->GetNextNode();	
	}
	return 0;
}

CDCL_INIFILE_ENTRY_NODE* CDCL_INIFILE::GetEntryNode(HDCL_INISEC hSec,char* szName)
{
	unsigned int nNumNodes=hSec->llEntries.GetNumNodes();
	CDCL_INIFILE_ENTRY_NODE* lpCurEnt=(CDCL_INIFILE_ENTRY_NODE*)hSec->llEntries.GetFirstNode();
	for(unsigned int i=0;i<nNumNodes;i++)
	{
		if(strcmp(lpCurEnt->szEntryName,szName)==0) return lpCurEnt;
		lpCurEnt=(CDCL_INIFILE_ENTRY_NODE*)lpCurEnt->GetNextNode();
	}
	
	if(lpBackupHook)
	{
		HDCL_INISEC hBakSec=lpBackupHook->GetSection(hSec->szSectionName);
		if(hBakSec==0) return 0;
		return lpBackupHook->GetEntryNode(hBakSec,szName);
	}
	return 0;
}

char* CDCL_INIFILE::GetStringValue(HDCL_INISEC hSec,char* szName)
{
	CDCL_INIFILE_ENTRY_NODE* lpEntry=GetEntryNode(hSec,szName);
	if(lpEntry==0) return 0;
	return lpEntry->szEntryValue;
}

int CDCL_INIFILE::GetIntValue(HDCL_INISEC hSec,char* szName)
{
    CDCL_INIFILE_ENTRY_NODE* lpEntry=GetEntryNode(hSec,szName);
    if(lpEntry==0) return 0;
	char* szStr=lpEntry->szEntryValue;
	if(szStr==0) return 0;
	
	return strtol(szStr,0,0);
}

char* CDCL_INIFILE::CutSpaces(char* szString)
{
	int nSLen=strlen(szString);
	char szTmp[]={szString[0],0};
	
	// Delete all whitespaces
	while(strpbrk(szTmp,"\v\t "))
	{
		szString++;
		nSLen--;
		szTmp[0]=szString[0];
	}
	
	// Delete whitespaces at the end of the string
	szTmp[0]=szString[nSLen-1];
	while(strpbrk(szTmp,"\v\t\r\n "))
	{
		szString[nSLen-1]=0;
		nSLen--;
		szTmp[0]=szString[nSLen-1];
	}
	
	return szString;
}

int CDCL_INIFILE::Unload()
{
	llSections.Free();
	return 1;
}

int CDCL_INIFILE::Load(char* szFile,EDCL_INIFILE nInitMode)
{
	nMode=nInitMode;
	
	FILE* file=fopen(szFile,"rt");
	if(file==0)
	{
		DCL_SetErrorStr("CDCL_INIFILE::Load -> Can't open file %s",szFile);
		return 0;
	}

	char 	szBuffer[256];
	char* 	szWBuffer;
	int 	nLines=0;
	
	CDCL_INIFILE_SECTION_NODE* lpCurSecNode=0;
	
	int nState=0;
	while(fgets(szBuffer,256,file)!=0)
	{
		szWBuffer=szBuffer;
		nLines++;
		
		// search for comment and remove it from the buffer
		char* szComment=strchr(szWBuffer,';');
		if(szComment)
		{
			szComment[0]=0;
		}
		
		szWBuffer=CutSpaces(szWBuffer);
		int nSLen=strlen(szWBuffer);
		
		// skip empty lines
		if(nSLen==0) continue;

		if(nSLen<3) // line is to short ?
		{
			DCL_SetErrorStr("CDCL_INIFILE::Load -> line %i of file %s is to short to be valid",
				nLines,szFile);
			fclose(file);
			return 0;
		}
		
		if(nState==0) // Global
		{
		    if(lpCurSecNode==0) // don't allow sectionless entries in global space !
			{
		 		if(szWBuffer[0]!='[')
		 		{
		 			DCL_SetErrorStr("CDCL_INIFILE::Load -> found 0x%X in %s line %i but expected '[' section start",
		 				szWBuffer[0],szFile,nLines);
		 			fclose(file);
		 			return 0;
		 		}
			}
			
			if(szWBuffer[0]=='[') // new section start ?
			{
				szWBuffer++;
				nSLen=strlen(szWBuffer);
				if(szWBuffer[nSLen-1]!=']') // section end not found ?
				{
					DCL_SetErrorStr("CDCL_INIFILE::Load -> found 0x%X in %s line %i but expected ']' end of section",
						szWBuffer[nSLen-1],szFile,nLines);
					fclose(file);
					return 0;
				}
				szWBuffer[nSLen-1]=0;
				nSLen--;
				lpCurSecNode=new CDCL_INIFILE_SECTION_NODE;
				lpCurSecNode->szSectionName=new char[nSLen+1];
				strcpy(lpCurSecNode->szSectionName,szWBuffer);
				
				llSections.AddNode(lpCurSecNode);
			}
			else // new entry start
			{
				char* szEqual=strchr(szWBuffer,'=');
				if(szEqual==0) // No equal sign found !
				{
				 	DCL_SetErrorStr("CDCL_INIFILE::Load -> entry without '=' in %s line %i",
				 		szFile,nLines);
				 	fclose(file);
				 	return 0;
				}
				char* szValue=szEqual+1;
				szEqual[0]=0;
				
				CutSpaces(szWBuffer);
				int nSLen=strlen(szWBuffer);
				
				if(nSLen==0) // entry name has zero length ?
				{
					DCL_SetErrorStr("CDCL_INIFILE::Load -> entry name has zero length in line %i of %s",
						nLines,szFile);
					fclose(file);
					return 0;
				}
				
				szValue=CutSpaces(szValue);
				int nVLen=strlen(szValue);
				
				if(nVLen==0) // entry value has zero length ?
				{
					DCL_SetErrorStr("CDCL_INIFILE::Load -> entry value has zero lenth in line %i of %s",
						nLines,szFile);
					fclose(file);
					return 0;
				}
				
				char* szIllegalName=strpbrk(szWBuffer,"\t[]");
			 	if(szIllegalName) // illegal character's in entry name ?
			 	{
			 	 	DCL_SetErrorStr("CDCL_INIFILE::Load -> illegal character 0x%X in entry name of %s line %i",
			 	 		szIllegalName[0],szFile,nLines);
			 	 	fclose(file);
			 	 	return 0;
			 	}
			 	char *szIllegalValue=strpbrk(szValue,"\t=");
			 	if(szIllegalValue) // nonvalid characters in entry value ?
			 	{
			 		DCL_SetErrorStr("CDCL_INIFILE::Load -> illegal character 0x%X in entry value of %s line %i",
			 			szIllegalValue[0],szFile,nLines);
			 		fclose(file);
			 		return 0;
			 	}
			 	
			 	// Finaly got 2 seperated strings with non zero length and legal chars in it !
			 	CDCL_INIFILE_ENTRY_NODE* lpEntry=new CDCL_INIFILE_ENTRY_NODE;
			 	lpEntry->szEntryName=new char[nSLen+1];
			 	strcpy(lpEntry->szEntryName,szWBuffer);
			 	
			 	lpEntry->szEntryValue=new char[nVLen+1];
			 	strcpy(lpEntry->szEntryValue,szValue);
			 	
			 	lpCurSecNode->llEntries.AddNode(lpEntry);
			}
		}
	}
	
	fclose(file);
	return 1;
}

CDCL_INIFILE* CDCL_INIFILE::GetBackupHook()
{
	return lpBackupHook;
}

CDCL_INIFILE* CDCL_INIFILE::SetBackupHook(CDCL_INIFILE* lpHook)
{
	CDCL_INIFILE* lpOldHook=lpBackupHook;
	lpBackupHook=lpHook;
	return lpOldHook;
}