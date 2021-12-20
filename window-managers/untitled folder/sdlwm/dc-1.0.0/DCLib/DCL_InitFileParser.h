#ifndef _DCL_INITFILEPARSER_H
#define _DCL_INITFILEPARSER_H

#include "DCL_DualLnkList.h"

enum EDCL_INIFILE
{
	DCLINI_NONCASESENSITIV,
	DCLINI_CASESENSITIV
};

class CDCL_INIFILE_ENTRY_NODE:public CDCL_DUALLNK_NODE
{
public:
    CDCL_INIFILE_ENTRY_NODE();
	~CDCL_INIFILE_ENTRY_NODE();
	char* 				szEntryName;
	char*				szEntryValue;
};

class CDCL_INIFILE_SECTION_NODE:public CDCL_DUALLNK_NODE
{
public:
    CDCL_INIFILE_SECTION_NODE();
	~CDCL_INIFILE_SECTION_NODE();
	char*				szSectionName;
	CDCL_DUALLNK_LIST	llEntries;
};

typedef CDCL_INIFILE_SECTION_NODE* HDCL_INISEC;

class CDCL_INIFILE
{
public:
	CDCL_INIFILE();
	~CDCL_INIFILE();
	CDCL_DUALLNK_LIST	llSections;

	int Load(char* szFile,EDCL_INIFILE nInitMode);
	int Unload();
	HDCL_INISEC GetSection(char* szName);
	char* GetStringValue(HDCL_INISEC hSec,char* szName);
	int GetIntValue(HDCL_INISEC hSec,char* szName);
	CDCL_INIFILE* SetBackupHook(CDCL_INIFILE* lpHook);
	CDCL_INIFILE* GetBackupHook();
private:
	CDCL_INIFILE_ENTRY_NODE* GetEntryNode(HDCL_INISEC hSec,char* szName);
	char* CutSpaces(char* szString);
	EDCL_INIFILE 	nMode;
	
	CDCL_INIFILE*	lpBackupHook;
};

#endif