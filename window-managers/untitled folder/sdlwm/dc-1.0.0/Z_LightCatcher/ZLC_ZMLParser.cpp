#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ZLC_RayTraceKernel.h"
#include "ZLC_ZMLParser.h"

/*
	<ZML> -> Z
		type= -> t "string"
		<CAMERA> -> C
			type= -> t "string"
		<SAMPLER> -> S
			type= -> t "string"
		<VIEWPORT> -> V
			type= -> t "string"
		<SHADER> -> H
			type= -> t "string"
		<AMBLIGHT> -> A
			color -> c "vec3"
		<SPHERE> -> P
			origin= -> o "vec3"
			radius= -> r "float"
			fraction= -> f "float"
			<M_COLOR> -> M
				type= -> r "string"
				color= -> c "vec3"
			<M_SOLIDNOISE> -> N
				type= -> r "string"
				color1= -> c "vec3"
				color2= -> d "vec3"
				scale= -> s "float"
		</OBJECT>
		<PLANE> -> E
			origin= -> o "vec3"
			direction= -> d "vec3"
			<M_COLOR> -> M
				type= -> r "string"
				color= -> c "vec3"
			<M_SOLIDNOISE> -> N
				type= -> r "string"
				color1= -> c "vec3"
				color2= -> d "vec3"
				scale= -> s "float"
		</OBJECT> -> O
		<L_POINT> -> L
		<L_PARALLEL> -> R
	</ZML>

	ACEHLMNPRSVZ

	(0x1) Can't open file %s
	(0x2) Unrecognized character (0x%X) in global space
	(0x3) Unrecognized global tag <%s>
	(0x4) Missing qutation marks after 'type' string
	(0x5) Missing qutation marks after 'color' vector
	(0x6) Unrecognized <ZML> tag <%s>
	(0x7) Missing qutation marks after 'origin' vector
	(0x8) Unrecognized character (0x%X) in <OBJECT> space
	(0x9) Unrecognized <OBJECT> tag <%s>
	(0xA) This is not a ZML file
	(0xB) ZML end tag not found
	(0xC) Unknown tag byte 0x%X
*/

void CZLC_ZMLPARSER::AddRedChar(char cCmd)
{
	if(nRedData>=nMaxRed)
	{
		nMaxRed+=256;
		szRedData=(char*)realloc(szRedData,nMaxRed);
	}
	szRedData[nRedData++]=cCmd;
}

bool CZLC_ZMLPARSER::GetZMLString(char* szType,char cTC,unsigned int& i,char* szLine)
{
	int nSL=strlen(szType);
	if(strncmp(szType,szLine+i,nSL)==0)
	{
		i+=nSL;

		AddRedChar(cTC);
		char* szES=strchr(szLine+i,'\"');
		if(szES==0)
		{
			nErrID=0x4;
			sprintf(szErrStr,"(0x%X) Missing qutation marks after zml string");
		}
		int nF=szES-(szLine+i);
		for(int j=0;j<nF;j++)
		{
			AddRedChar(szLine[i++]);
		}
		AddRedChar('\"');
		return true;
	}
	return false;
}

bool CZLC_ZMLPARSER::GetZMLTag(char* szType,char cTC,unsigned int& i,char* szLine,int nNewState)
{
	int nSL=strlen(szType);
	if(strncmp(szType,szLine+i,nSL)==0)
	{
		i+=nSL;
		AddRedChar(cTC);
		nState=nNewState;
		return true;
	}
	return false;
}

bool CZLC_ZMLPARSER::GetZMLVector(char* szType,char cTC,unsigned int& i,char* szLine)
{
	int nSL=strlen(szType);
	if(strncmp(szType,szLine+i,nSL)==0)
	{
		i+=nSL;
		AddRedChar(cTC);
		char* szEV=strchr(szLine+i,')');
		if(szEV==0)
		{
			nErrID=0x5;
			sprintf(szErrStr,"(0x%X) Missing closing bracet after zml vector");
		}
		int nF=szEV-(szLine+i);
		for(int j=0;j<nF;j++)
		{
			AddRedChar(szLine[i++]);
		}
		AddRedChar('\"');
		return true;
	}
	return false;
}

bool CZLC_ZMLPARSER::GetZMLFloat(char* szType,char cTC,unsigned int& i,char* szLine)
{
	int nSL=strlen(szType);
	if(strncmp(szType,szLine+i,nSL)==0)
	{
		i+=nSL;
		AddRedChar(cTC);
		while((!isalpha(szLine[i]))&&szLine[i]!='>'&&szLine[i]!='\n'&&szLine[i]!='\0')
		{
			AddRedChar(szLine[i++]);
		}
		AddRedChar('\"');
		i--;
		return true;
	}
	return false;
}

int CZLC_ZMLPARSER::ReduceLine(char* szLine)
{
	for(unsigned int i=0;i<strlen(szLine);i++)
	{
		switch(nState)
		{
		case 0:
			{
				while(szLine[i]==' '||szLine[i]=='\t'||szLine[i]=='\n') {i++;continue;}
				if(szLine[i]==0) return 0;
				if(szLine[i]=='<') nState=1;
				else
				{
					nErrID=0x2;
					sprintf(szErrStr,"(0x%X) Unrecognized character (0x%X) in global space",nErrID,szLine[i]);
					return nErrID;
				}
				break;
			}
		case 1:
			{
				while(szLine[i]==' '||szLine[i]=='\t'||szLine[i]=='\n') {i++;continue;}
				if(szLine[i]==0) return 0;
				if(GetZMLTag("ZML",'Z',i,szLine,2)) {}
				else
				{
					nErrID=0x3;
					char szTag[256]={0};
					char* szWS=strpbrk(szLine+i,"\t\n ");
					int nWS=szWS?szWS-(szLine+i):strlen(szLine);
					strncpy(szTag,szLine+i,nWS);
					sprintf(szErrStr,"(0x%X) Unrecognized global tag <%s>",nErrID,szTag);
					return nErrID;
				}
				break;
			}
		case 2:
			{
				while(szLine[i]==' '||szLine[i]=='\t'||szLine[i]=='\n') {i++;continue;}
				if(szLine[i]==0) return 0;

				if(GetZMLString("type=\"",'t',i,szLine)) {}
				else if(szLine[i]=='>') nState=3;
				break;
			}
		case 3:
			{
				while(szLine[i]==' '||szLine[i]=='\t'||szLine[i]=='\n') {i++;continue;}
				if(szLine[i]==0) return 0;
				if(szLine[i]=='<') nState=4;
				else
				{
					nErrID=0x2;
					sprintf(szErrStr,"(0x%X) Unrecognized character (0x%X) in <ZML> space",nErrID,szLine[i]);
					return nErrID;
				}
				break;
			}
		case 4:
			{
				while(szLine[i]==' '||szLine[i]=='\t'||szLine[i]=='\n') {i++;continue;}
				if(szLine[i]==0) return 0;
				
				if(GetZMLTag("/ZML",0,i,szLine,0)) {}
				else if(GetZMLTag("CAMERA",'C',i,szLine,5)) {}
				else if(GetZMLTag("SAMPLER",'S',i,szLine,5)) {}
				else if(GetZMLTag("VIEWPORT",'V',i,szLine,15)) {}
				else if(GetZMLTag("SHADER",'H',i,szLine,5)) {}
				else if(GetZMLTag("AMBLIGHT",'A',i,szLine,6)) {}
				else if(GetZMLTag("SPHERE",'P',i,szLine,7)) {}
				else if(GetZMLTag("PLANE",'E',i,szLine,12)) {}
				else if(GetZMLTag("L_POINT",'L',i,szLine,13)) {}
				else if(GetZMLTag("L_PARALLEL",'R',i,szLine,14)) {}
				else 
				{
					nErrID=0x6;
					char szTag[256]={0};
					char* szWS=strpbrk(szLine+i,"\t\n ");
					int nWS=szWS?szWS-(szLine+i):strlen(szLine);
					strncpy(szTag,szLine+i,nWS);
					sprintf(szErrStr,"(0x%X) Unrecognized <ZML> tag <%s>",nErrID,szTag);
					return nErrID;
				}
				break;
			}
		case 5:
			{
				while(szLine[i]==' '||szLine[i]=='\t'||szLine[i]=='\n') {i++;continue;}
				if(szLine[i]==0) return 0;

				if(GetZMLString("type=\"",'t',i,szLine)) {}
				else if(szLine[i]=='>') nState=3;
				break;
			}
		case 6:
			{
				while(szLine[i]==' '||szLine[i]=='\t'||szLine[i]=='\n') {i++;continue;}
				if(szLine[i]==0) return 0;
				if(GetZMLVector("color=(",'c',i,szLine)) {}
				else if(szLine[i]=='>')	nState=3;
				break;
			}
		case 7:
			{
				while(szLine[i]==' '||szLine[i]=='\t'||szLine[i]=='\n') {i++;continue;}
				if(szLine[i]==0) return 0;

				if(GetZMLVector("origin=(",'o',i,szLine)) {}
				else if(GetZMLFloat("radius=",'r',i,szLine)) {}
				else if(GetZMLFloat("fraction=",'f',i,szLine)) {}
				else if(szLine[i]=='>') nState=8;
				break;
			}
		case 8:
			{
				while(szLine[i]==' '||szLine[i]=='\t'||szLine[i]=='\n') {i++;continue;}
				if(szLine[i]==0) return 0;

				if(szLine[i]=='<') nState=9;
				else
				{
					nErrID=0x8;
					sprintf(szErrStr,"(0x%X) Unrecognized character (0x%X) in <OBJECT> space",nErrID,szLine[i]);
					return nErrID;
				}
				break;
			}
		case 9:
			{
				while(szLine[i]==' '||szLine[i]=='\t'||szLine[i]=='\n') {i++;continue;}
				if(szLine[i]==0) return 0;

				if(GetZMLTag("M_COLOR",'M',i,szLine,10)) {}
				else if(GetZMLTag("M_SOLIDNOISE",'N',i,szLine,11)) {}
				else if(GetZMLTag("/OBJECT",'O',i,szLine,3)) {}
				else
				{
					nErrID=0x9;
					char szTag[256]={0};
					char* szWS=strpbrk(szLine+i,"\t\n ");
					int nWS=szWS?szWS-(szLine+i):strlen(szLine);
					strncpy(szTag,szLine+i,nWS);
					sprintf(szErrStr,"(0x%X) Unrecognized <OBJECT> tag <%s>",nErrID,szTag);
					return nErrID;
				}
				break;
			}
		case 10:
			{
				while(szLine[i]==' '||szLine[i]=='\t'||szLine[i]=='\n') {i++;continue;}
				if(szLine[i]==0) return 0;

				if(GetZMLString("type=\"",'t',i,szLine)) {}
				else if(GetZMLVector("color=(",'c',i,szLine)) {}
				else if(szLine[i]=='>') nState=8;
				break;
			}
		case 11:
			{
				while(szLine[i]==' '||szLine[i]=='\t'||szLine[i]=='\n') {i++;continue;}
				if(szLine[i]==0) return 0;

				if(GetZMLString("type=\"",'t',i,szLine)) {}
				else if(GetZMLVector("color1=(",'c',i,szLine)) {}
				else if(GetZMLVector("color2=(",'d',i,szLine)) {}
				else if(GetZMLFloat("scale=",'s',i,szLine)) {}
				else if(szLine[i]=='>') nState=8;
				break;
			}
		case 12:
			{
				while(szLine[i]==' '||szLine[i]=='\t'||szLine[i]=='\n') {i++;continue;}
				if(szLine[i]==0) return 0;

				if(GetZMLVector("origin=(",'o',i,szLine)) {}
				else if(GetZMLVector("direction=(",'d',i,szLine)) {}
				else if(szLine[i]=='>') nState=8;
				break;
			}
		case 13:
			{
				while(szLine[i]==' '||szLine[i]=='\t'||szLine[i]=='\n') {i++;continue;}
				if(szLine[i]==0) return 0;
				
				if(GetZMLVector("position=(",'p',i,szLine)) {}
				else if(GetZMLVector("color=(",'c',i,szLine)) {}
				else if(szLine[i]=='>') nState=3;
				break;
			}
		case 14:
			{
				while(szLine[i]==' '||szLine[i]=='\t'||szLine[i]=='\n') {i++;continue;}
				if(szLine[i]==0) return 0;

				if(GetZMLVector("direction=(",'d',i,szLine)) {}
				else if(GetZMLVector("color=(",'c',i,szLine)) {}
				else if(szLine[i]=='>')	nState=3;
				break;
			}
		case 15:
			{
				while(szLine[i]==' '||szLine[i]=='\t'||szLine[i]=='\n') {i++;continue;}
				if(szLine[i]==0) return 0;

				if(GetZMLFloat("w=",'w',i,szLine)) {}
				else if(GetZMLFloat("h=",'h',i,szLine)) {}
				else if(szLine[i]=='>') nState=3;
				break;
			}
		}
	}
	return 0;
}

int CZLC_ZMLPARSER::ReduceFile(FILE* file)
{
	
	char szBuffer[256]={0};
	while(fgets(szBuffer,256,file))
	{
		if(ReduceLine(szBuffer)!=0)	return nErrID;
	}
	AddRedChar(0);
	return 0;
}

int CZLC_ZMLPARSER::Parse(char* szFile)
{
	FILE* file=fopen(szFile,"rt");
	if(file==0) 
	{
		nErrID=0x1;
		sprintf(szErrStr,"(0x%X) Can't open file %s",nErrID,szFile);
		return nErrID;
	}

	if(ReduceFile(file)!=0)
	{
		fclose(file);
		return nErrID;
	}

	int nState=0;

	if(szRedData[0]!='Z')
	{
		nErrID=0xA;
		sprintf(szErrStr,"(0x%X) This is not a ZML file",nErrID);
		return nErrID;
	}
	else if(szRedData[nRedData-1]!=0)
	{
		nErrID=0xB;
		sprintf(szErrStr,"(0x%X) ZML end tag not found",nErrID);
		return nErrID;
	}

	fclose(file);
	return 0;
}

CZLC_SCENE* CZLC_ZMLPARSER::LoadScene()
{
	CZLC_SCENE* lpNewScene=new CZLC_SCENE();

	char szType[64]={0};

	int nVW=0,nVH=0;

	CZLA_VECTOR4 vecTmp1,vecTmp2;
	CZLC_COLOR colTmp1,colTmp2;
	ZLA_FLOAT fTmp1,fTmp2;
	CZLA_OBJECT* lpTmpObj;
	int nTmp1;

	int nParams=0;
	char szBuffer[256];

	int nState=0;
	int i=0;
	while(szRedData[i]!=0)
	{
		switch(nState)
		{
		case 0:
			if(szRedData[i]=='Z') {nState=1;i++;}
			break;
		case 1:
			if(szRedData[i]=='t')
			{
				char* szQM=strchr(szRedData+i,'\"');
				int nL=szQM-(szRedData+i);
				strncpy(szType,szRedData+i+1,nL-1);
				i+=nL+1;
			}
			else nState=2;
			break;
		case 2:
			if     (szRedData[i]=='C') {i++;nState=3;}
			else if(szRedData[i]=='S') {i++;nState=4;}
			else if(szRedData[i]=='V') {i++;nState=5;}
			else if(szRedData[i]=='H') {i++;nState=6;}
			else if(szRedData[i]=='A') {i++;nState=7;}
			else if(szRedData[i]=='P') {i++;nState=8;nParams=0;}
			else if(szRedData[i]=='E') {i++;nState=11;nParams=0;}
			else if(szRedData[i]=='L') {i++;nState=12;nParams=0;}
			else if(szRedData[i]=='R') {i++;nState=13;nParams=0;}
			else 
			{
				nErrID=0xC;
				sprintf(szErrStr,"(0x%X) Unknown tag byte 0x%X",nErrID,szRedData[i]);
				return 0;
			}
			break;
		case 3:
			if(szRedData[i]=='t')
			{
				char* szQM=strchr(szRedData+i,'\"');
				int nL=szQM-(szRedData+i);
				strncpy(szBuffer,szRedData+i+1,nL-1);
				szBuffer[nL-1]=0;
				i+=nL+1;

				if(strcmp(szBuffer,"pinhole")==0)
				{
					lpNewScene->lpCamera=new CZLC_CAM_PINHOLE;
				}
				else if(strcmp(szBuffer,"orthogonal")==0)
				{
					lpNewScene->lpCamera=new CZLC_CAM_ORTHOGONAL;
				}
			}
			else
			{
				nState=2;
			}
			break;
		case 4:
			if(szRedData[i]=='t')
			{
				char* szQM=strchr(szRedData+i,'\"');
				int nL=szQM-(szRedData+i);
				strncpy(szBuffer,szRedData+i+1,nL-1);
				szBuffer[nL-1]=0;
				i+=nL+1;

				if(strcmp(szBuffer,"singleray")==0)
				{
					lpNewScene->lpSampler=new CZLC_SMP_SINGLERAY;
				}
				else if(strcmp(szBuffer,"polyray")==0)
				{
					lpNewScene->lpSampler=new CZLC_SMP_POLYRAY;
				}
			}
			else
			{
				nState=2;
			}
			break;
		case 5:
			if(szRedData[i]=='w')
			{
				char* szQM=strchr(szRedData+i,'\"');
				int nL=szQM-(szRedData+i);
				strncpy(szBuffer,szRedData+i+1,nL-1);
				szBuffer[nL-1]=0;
				i+=nL+1;

				nVW=atoi(szBuffer);
			}
			else if(szRedData[i]=='h')
			{
				char* szQM=strchr(szRedData+i,'\"');
				int nL=szQM-(szRedData+i);
				strncpy(szBuffer,szRedData+i+1,nL-1);
				szBuffer[nL-1]=0;
				i+=nL+1;

				nVH=atoi(szBuffer);
			}
			else nState=2;

			if(nVW&&nVH)
			{
				lpNewScene->lpViewPort=new CZLC_VIEWPORT(nVW,nVH);
				nVW=0;nVH=0;
			}
			break; 
		case 6:
			if(szRedData[i]=='t')
			{
				char* szQM=strchr(szRedData+i,'\"');
				int nL=szQM-(szRedData+i);
				strncpy(szBuffer,szRedData+i+1,nL-1);
				szBuffer[nL-1]=0;
				i+=nL+1;

				if(strcmp(szBuffer,"phong")==0)
				{
					lpNewScene->lpShader=new CZLC_SHD_PHONG();
				}
			}
			else nState=2;
			break;
		case 7:
			if(szRedData[i]=='c')
			{
				char* szQM=strchr(szRedData+i,'\"');
				int nL=szQM-(szRedData+i);
				strncpy(szBuffer,szRedData+i+1,nL-1);
				szBuffer[nL-1]=0;
				i+=nL+1;

				float fA,fB,fC;
				sscanf(szBuffer,"%f,%f,%f",&fA,&fB,&fC);
				lpNewScene->cAmb=CZLC_COLOR(fA,fB,fC);
			}
			else nState=2;
			break;
		case 8:
			{
				if(szRedData[i]=='o')
				{
					char* szQM=strchr(szRedData+i,'\"');
					int nL=szQM-(szRedData+i);
					strncpy(szBuffer,szRedData+i+1,nL-1);
					szBuffer[nL-1]=0;
					i+=nL+1;

					float fA,fB,fC;
					sscanf(szBuffer,"%f,%f,%f",&fA,&fB,&fC);

					vecTmp1=CZLA_VECTOR4(fA,fB,fC,1);
					nParams|=1;
				}
				else if(szRedData[i]=='r')
				{
					char* szQM=strchr(szRedData+i,'\"');
					int nL=szQM-(szRedData+i);
					strncpy(szBuffer,szRedData+i+1,nL-1);
					szBuffer[nL-1]=0;
					i+=nL+1;

					fTmp1=(ZLA_FLOAT)atof(szBuffer);
					nParams|=2;
				}
				else if(szRedData[i]=='f')
				{
					char* szQM=strchr(szRedData+i,'\"');
					int nL=szQM-(szRedData+i);
					strncpy(szBuffer,szRedData+i+1,nL-1);
					szBuffer[nL-1]=0;
					i+=nL+1;

					fTmp2=(ZLA_FLOAT)atof(szBuffer);
					nParams|=4;
				}
				else if(szRedData[i]=='O')
				{
					lpTmpObj=0;
					i++;
					nState=2;
				}
				else if(szRedData[i]=='M') {i++;nState= 9;nParams=0;}
				else if(szRedData[i]=='N') {i++;nState=10;nParams=0;}

				if(nParams==7)
				{
					nParams=0;
					lpTmpObj=new CZLA_SPHERE(vecTmp1,fTmp1);
					lpTmpObj->fN=fTmp2;
					lpNewScene->cDataSet.llObjects.AddNode(lpTmpObj);
				}
				break;
			}
		case 9:
			{
				if(szRedData[i]=='t')
				{
					char* szQM=strchr(szRedData+i,'\"');
					int nL=szQM-(szRedData+i);
					strncpy(szBuffer,szRedData+i+1,nL-1);
					szBuffer[nL-1]=0;
					i+=nL+1;
				
					if(strcmp(szBuffer,"diffuse")==0) nTmp1=EZLAM_DIFFUSE;
					else if(strcmp(szBuffer,"specular")==0) nTmp1=EZLAM_SPECULAR;
					else if(strcmp(szBuffer,"transparent")==0) nTmp1=EZLAM_TRANSPARENT;
					nParams|=1;
				}
				else if(szRedData[i]=='c')
				{
					char* szQM=strchr(szRedData+i,'\"');
					int nL=szQM-(szRedData+i);
					strncpy(szBuffer,szRedData+i+1,nL-1);
					szBuffer[nL-1]=0;
					i+=nL+1;

					float fA,fB,fC;
					sscanf(szBuffer,"%f,%f,%f",&fA,&fB,&fC);
					colTmp1=CZLC_COLOR(fA,fB,fC);
					nParams|=2;
				}
				else nState=8;
				if(nParams==3)
				{
					nParams=0;
					lpTmpObj->SetMaterial(nTmp1,new CZLA_MATERIAL(colTmp1));
				}
				break;
			}
		case 10:
			{
				if(szRedData[i]=='t')
				{
					char* szQM=strchr(szRedData+i,'\"');
					int nL=szQM-(szRedData+i);
					strncpy(szBuffer,szRedData+i+1,nL-1);
					szBuffer[nL-1]=0;
					i+=nL+1;
				
					if(strcmp(szBuffer,"diffuse")==0) nTmp1=EZLAM_DIFFUSE;
					else if(strcmp(szBuffer,"specular")==0) nTmp1=EZLAM_SPECULAR;
					else if(strcmp(szBuffer,"transparent")==0) nTmp1=EZLAM_TRANSPARENT;
					nParams|=1;
				}
				else if(szRedData[i]=='c')
				{
					char* szQM=strchr(szRedData+i,'\"');
					int nL=szQM-(szRedData+i);
					strncpy(szBuffer,szRedData+i+1,nL-1);
					szBuffer[nL-1]=0;
					i+=nL+1;

					float fA,fB,fC;
					sscanf(szBuffer,"%f,%f,%f",&fA,&fB,&fC);
					colTmp1=CZLC_COLOR(fA,fB,fC);
					nParams|=2;
				}
				else if(szRedData[i]=='d')
				{
					char* szQM=strchr(szRedData+i,'\"');
					int nL=szQM-(szRedData+i);
					strncpy(szBuffer,szRedData+i+1,nL-1);
					szBuffer[nL-1]=0;
					i+=nL+1;

					float fA,fB,fC;
					sscanf(szBuffer,"%f,%f,%f",&fA,&fB,&fC);
					colTmp2=CZLC_COLOR(fA,fB,fC);
					nParams|=4;
				}
				else if(szRedData[i]=='s')
				{
					char* szQM=strchr(szRedData+i,'\"');
					int nL=szQM-(szRedData+i);
					strncpy(szBuffer,szRedData+i+1,nL-1);
					szBuffer[nL-1]=0;
					i+=nL+1;

					fTmp1=(ZLA_FLOAT)atof(szBuffer);
					nParams|=8;
				}
				else nState=8;

				if(nParams==15)
				{
					nParams=0;
					lpTmpObj->SetMaterial(nTmp1,new 
						CZLA_MAT_SOLIDNOISE(colTmp1,colTmp2,fTmp1));
				}
				break;
			}
		case 11:
			{
				if(szRedData[i]=='o')
				{
					char* szQM=strchr(szRedData+i,'\"');
					int nL=szQM-(szRedData+i);
					strncpy(szBuffer,szRedData+i+1,nL-1);
					szBuffer[nL-1]=0;
					i+=nL+1;

					float fA,fB,fC;
					sscanf(szBuffer,"%f,%f,%f",&fA,&fB,&fC);

					vecTmp1=CZLA_VECTOR4(fA,fB,fC,1);
					nParams|=1;
				}
				else if(szRedData[i]=='d')
				{
					char* szQM=strchr(szRedData+i,'\"');
					int nL=szQM-(szRedData+i);
					strncpy(szBuffer,szRedData+i+1,nL-1);
					szBuffer[nL-1]=0;
					i+=nL+1;

					float fA,fB,fC;
					sscanf(szBuffer,"%f,%f,%f",&fA,&fB,&fC);

					vecTmp2=CZLA_VECTOR4(fA,fB,fC,1);
					nParams|=2;
				}
				else if(szRedData[i]=='O') {i++;nState=2;}
				else if(szRedData[i]=='M') {i++;nState=9;nParams=0;}
				else if(szRedData[i]=='N') {i++;nState=10;nParams=0;}

				if(nParams==3)
				{
					nParams=0;
					lpTmpObj=new CZLA_PLANE(vecTmp1,vecTmp2);
					lpNewScene->cDataSet.llObjects.AddNode(lpTmpObj);
				}
				break;
			}
		case 12:
			{
				if(szRedData[i]=='p')
				{
					char* szQM=strchr(szRedData+i,'\"');
					int nL=szQM-(szRedData+i);
					strncpy(szBuffer,szRedData+i+1,nL-1);
					szBuffer[nL-1]=0;
					i+=nL+1;

					float fA,fB,fC;
					sscanf(szBuffer,"%f,%f,%f",&fA,&fB,&fC);

					vecTmp1=CZLA_VECTOR4(fA,fB,fC,1);
					nParams|=1;
				}
				else if(szRedData[i]=='c')
				{
					char* szQM=strchr(szRedData+i,'\"');
					int nL=szQM-(szRedData+i);
					strncpy(szBuffer,szRedData+i+1,nL-1);
					szBuffer[nL-1]=0;
					i+=nL+1;

					float fA,fB,fC;
					sscanf(szBuffer,"%f,%f,%f",&fA,&fB,&fC);

					colTmp1=CZLC_COLOR(fA,fB,fC);
					nParams|=2;
				}
				else nState=2;

				if(nParams==3)
				{
					nParams=0;
					CZLA_LT_POINT* lpPointLt1=
						new CZLA_LT_POINT(vecTmp1,colTmp1);
					lpNewScene->cDataSet.llLight.AddNode(lpPointLt1);
				}
				break;
			}
		case 13:
			{
				if(szRedData[i]=='d')
				{
					char* szQM=strchr(szRedData+i,'\"');
					int nL=szQM-(szRedData+i);
					strncpy(szBuffer,szRedData+i+1,nL-1);
					szBuffer[nL-1]=0;
					i+=nL+1;

					float fA,fB,fC;
					sscanf(szBuffer,"%f,%f,%f",&fA,&fB,&fC);

					vecTmp1=CZLA_VECTOR4(fA,fB,fC,1);
					nParams|=1;
				}
				else if(szRedData[i]=='c')
				{
					char* szQM=strchr(szRedData+i,'\"');
					int nL=szQM-(szRedData+i);
					strncpy(szBuffer,szRedData+i+1,nL-1);
					szBuffer[nL-1]=0;
					i+=nL+1;

					float fA,fB,fC;
					sscanf(szBuffer,"%f,%f,%f",&fA,&fB,&fC);

					colTmp1=CZLC_COLOR(fA,fB,fC);
					nParams|=2;
				}
				else nState=2;

				if(nParams==3)
				{
					nParams=0;
					CZLA_LT_PARALLEL* lpParLt1=
						new CZLA_LT_PARALLEL(vecTmp1,colTmp1);
					lpNewScene->cDataSet.llLight.AddNode(lpParLt1);
				}
				break;
			}
		}
	}
	return lpNewScene;
}

CZLC_ZMLPARSER::CZLC_ZMLPARSER():nErrID(0),nRedData(0),szRedData(0),nMaxRed(0),nState(0)
{
	
}

CZLC_ZMLPARSER::~CZLC_ZMLPARSER() 
{
	if(szRedData) free(szRedData);
}

void CZLC_ZMLPARSER::CleanUp()
{
	if(szRedData) free(szRedData);
	szRedData=0;
	nRedData=0;
	nMaxRed=0;
	nState=0;
	nErrID=0;
}

char* CZLC_ZMLPARSER::GetErrString()
{
	return szErrStr;
}