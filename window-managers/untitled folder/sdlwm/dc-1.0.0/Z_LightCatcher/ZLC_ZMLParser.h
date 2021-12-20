#ifndef _ZLC_ZMLPARSER_H
#define _ZLC_ZMLPARSER_H

#include "ZLC_Scene.h"

class CZLC_ZMLPARSER
{
public:
	int Parse(char* szFile);
	CZLC_SCENE* LoadScene();

	int ReduceFile(FILE* file);
	int ReduceLine(char* szLine);

	bool GetZMLString(char* szType,char cTC,unsigned int& i,char* szLine);
	bool GetZMLTag(char* szType,char cTC,unsigned int& i,char* szLine,int nNewState);
	bool GetZMLVector(char* szType,char cTC,unsigned int& i,char* szLine);
	bool GetZMLFloat(char* szType,char cTC,unsigned int& i,char* szLine);
	void AddRedChar(char cCmd);
	void CleanUp();
	char* GetErrString();

	CZLC_ZMLPARSER();
	~CZLC_ZMLPARSER();

	char* szRedData;
	int nRedData;
	int nMaxRed;
	int nState;
	

private:
	char	szErrStr[256];
	int		nErrID;
};

#endif