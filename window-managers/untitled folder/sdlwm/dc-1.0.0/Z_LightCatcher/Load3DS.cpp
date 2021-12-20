#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Load3DS.h"

#define MAX_RECD 5000

DSUINT32 GetUINT32(FILE* file)
{
	DSUINT32 nRet;
	fread(&nRet,sizeof(nRet),1,file);
	return nRet;
}

DSFLOAT32 GetFLOAT32(FILE* file)
{
	DSFLOAT32 nRet;
	fread(&nRet,sizeof(nRet),1,file);
	return nRet;
}

DSUINT16 GetUINT16(FILE* file)
{
	DSUINT16 nRet;
	fread(&nRet,sizeof(nRet),1,file);
	return nRet;
}

DSUINT8 GetUINT8(FILE* file)
{
	DSUINT8 nRet;
	fread(&nRet,sizeof(nRet),1,file);
	return nRet;
}

void GetString(FILE* file,char* szStr,int nLen)
{
	for(int i=0;i<nLen;i++)
	{
		char c=getc(file);
		szStr[i]=c;
		if(c=='\0') break;
	}
}

void SkipChunk(FILE* file,DSUINT16 nID,DSUINT32 nSize)
{
	fseek(file,nSize,SEEK_CUR);
	printf("#### Skipping chunk 0x%X size : %i\n",nID,nSize);
}

void C3DS_OBJECT::ReadPercentContainerChunk(DSUINT16 nID,DSUINT32 nSize)
{
	int nDCnt=0;
	while(nSize)
	{
		if(MAX_RECD<=nDCnt)
		{
			printf("**** Reached MaxD in PercentContainer Chunk\n");
			break;
		}

		DSUINT16 nL_ID=GetUINT16(file);
		DSUINT32 nL_Size=GetUINT32(file);

		DSUINT16*	lpCurPer;
		switch(nID)
		{
		case 0xA040: lpCurPer=&lpLastMat->pShiny; break;
		case 0xA041: lpCurPer=&lpLastMat->pShinyStr; break;
		case 0xA050: lpCurPer=&lpLastMat->pTrans; break;
		case 0xA052: lpCurPer=&lpLastMat->pTransFall; break;
		case 0xA053: lpCurPer=&lpLastMat->pRefBlur; break;
		case 0xA084: lpCurPer=&lpLastMat->pSelfIlu; break;
		}

		switch(nL_ID)
		{
		case 0x0030:
			{
				*lpCurPer=GetUINT16(file);
				printf("UINT16 %i\n",*lpCurPer);
				nSize-=nL_Size;
			}
			break;
		default:
			printf("**** Unknown PercentContainer Chunk :)\n");
			SkipChunk(file,nL_ID,nL_Size-6);
			nSize-=nL_Size;
			break;
		}
		nDCnt++;
	}
}

void C3DS_OBJECT::ReadColorContainerChunk(DSUINT16 nID,DSUINT32 nSize)
{
	int nDCnt=0;
	while(nSize)
	{
		if(MAX_RECD<=nDCnt)
		{
			printf("**** Reached MaxD in ColorContainer Chunk\n");
			break;
		}

		DSUINT16 nL_ID=GetUINT16(file);
		DSUINT32 nL_Size=GetUINT32(file);

		UI8RGB*	lpCurCol;
		switch(nID)
		{
		case 0xA010: lpCurCol=&lpLastMat->cAmb; break;
		case 0xA020: lpCurCol=&lpLastMat->cDiff; break;
		case 0xA030: lpCurCol=&lpLastMat->cSpec; break;
		}

		switch(nL_ID)
		{
		case 0x0011:
			{
				lpCurCol->r=GetUINT8(file);
				lpCurCol->g=GetUINT8(file);
				lpCurCol->b=GetUINT8(file);
				printf("Byte RGB(%i,%i,%i)\n",lpCurCol->r,lpCurCol->g,lpCurCol->b);
				nSize-=nL_Size;
			}
			break;
		default:
			printf("**** Unknown ColorContainer Chunk :)\n");
			SkipChunk(file,nL_ID,nL_Size-6);
			nSize-=nL_Size;
			break;
		}
		nDCnt++;
	}
}

void C3DS_OBJECT::ReadMapChunk(DSUINT16 nID,DSUINT32 nSize)
{
	int nDCnt=0;
	while(nSize)
	{
		if(MAX_RECD<=nDCnt)
		{
			printf("**** Reached MaxD in Map Chunk\n");
			break;
		}

		DSUINT16 nL_ID=GetUINT16(file);
		DSUINT32 nL_Size=GetUINT32(file);

		switch(nL_ID)
		{
		case 0x0030:
			{
				DSUINT16 wPer=GetUINT16(file);
				printf("Map Percentage UINT16 %i\n",wPer);
				nSize-=nL_Size;
			}
			break;
		case 0xA300:
			{
				char szBuffer[256];
				GetString(file,szBuffer,nL_Size-6);
				lpLastMat->szFNam=strdup(szBuffer);
				printf("MapFile (%s)\n",szBuffer);
				nSize-=nL_Size;	
			}
			break;
		case 0xA351:
			{
				DSUINT16 wFlg=GetUINT16(file);
				printf("Map Flags 0x%X\n",wFlg);
				nSize-=nL_Size;
			}
			break;
		case 0xA353:
			{
				DSFLOAT32 fMapBlur=GetFLOAT32(file);
				printf("Map Bluring : %f\n",fMapBlur);
				nSize-=nL_Size;
			}
			break;
		default:
			printf("**** Unknown Map Chunk :)\n");
			SkipChunk(file,nL_ID,nL_Size-6);
			nSize-=nL_Size;
			break;
		}
		nDCnt++;
	}
}

void C3DS_OBJECT::ReadMaterialChunk(DSUINT16 nID,DSUINT32 nSize)
{
	int nDCnt=0;
	while(nSize)
	{
		if(MAX_RECD<=nDCnt)
		{
			printf("**** Reached MaxD in Material Chunk\n");
			break;
		}

		DSUINT16 nL_ID=GetUINT16(file);
		DSUINT32 nL_Size=GetUINT32(file);

		if((nL_ID>=0xA200)&&(nL_ID<=0xA34C))
		{
			ReadMapChunk(nL_ID,nL_Size-6);
			nSize-=nL_Size;
		}
		else
		{
			switch(nL_ID)
			{
			case 0xA000:
				{
					DS_MAT* lpNew=new DS_MAT();
					memset(lpNew,0,sizeof(DS_MAT));
					if(lpLastMat==0)
					{
						lpLastMat=lpNew;
						lpFirstMat=lpNew;
						lpNew->lpNext=0;
						lpNew->lpPrev=0;
					}
					else
					{
						lpLastMat->lpNext=lpNew;
						lpNew->lpPrev=lpLastMat;
						lpNew->lpNext=0;
						lpLastMat=lpNew;
					}
					dwNumMat++;

					char szBuffer[256];
					GetString(file,szBuffer,nL_Size-6);
					printf("Material Name (%s)\n",szBuffer);
					lpLastMat->szName=strdup(szBuffer);
					nSize-=nL_Size;
					break;
				}
			case 0xA010:
				printf("Material Ambient  Color : ");
				ReadColorContainerChunk(nL_ID,nL_Size-6);
				nSize-=nL_Size;
				break;
			case 0xA020:
				printf("Material Diffuse  Color : ");
				ReadColorContainerChunk(nL_ID,nL_Size-6);
				nSize-=nL_Size;
				break;
			case 0xA030:
				printf("Material Specular Color : ");
				ReadColorContainerChunk(nL_ID,nL_Size-6);
				nSize-=nL_Size;
				break;
			case 0xA040:
				printf("Material Shininess Percent : ");
				ReadPercentContainerChunk(nL_ID,nL_Size-6);
				nSize-=nL_Size;
				break;
			case 0xA041:
				printf("Material Shininess Strength Percent : ");
				ReadPercentContainerChunk(nL_ID,nL_Size-6);
				nSize-=nL_Size;
				break;
			case 0xA050:
				printf("Material Transparency Percent : ");
				ReadPercentContainerChunk(nL_ID,nL_Size-6);
				nSize-=nL_Size;
				break;
			case 0xA052:
				printf("Material Transparency Falloff Percent : ");
				ReadPercentContainerChunk(nL_ID,nL_Size-6);
				nSize-=nL_Size;
				break;
			case 0xA053:
				printf("Material Reflection Blur Percent : ");
				ReadPercentContainerChunk(nL_ID,nL_Size-6);
				nSize-=nL_Size;
				break;
			case 0xA100:
				{
					lpLastMat->wRenderType=GetUINT16(file);
					printf("RenderType : %i\n",lpLastMat->wRenderType);
					nSize-=nL_Size;
				}
				break;
			case 0xA084:
				printf("Material Self Ilumination Percent : ");
				ReadPercentContainerChunk(nL_ID,nL_Size-6);
				nSize-=nL_Size;
				break;
			case 0xA087:
				{
					lpLastMat->fWireThickness=GetFLOAT32(file);
					printf("Wire Thickness : %f\n",lpLastMat->fWireThickness);
					nSize-=nL_Size;
				}
				break;
			case 0xA08A:
				printf("---- In tranc Chunk ?\n");
				SkipChunk(file,nL_ID,nL_Size-6);
				nSize-=nL_Size;
				break;
			default:
				printf("**** Unknown Material Chunk :)\n");
				SkipChunk(file,nL_ID,nL_Size-6);
				nSize-=nL_Size;
				break;
			}
		}
		nDCnt++;
	}
}

void C3DS_OBJECT::ReadFacesChunk(DSUINT16 nID,DSUINT32 nSize)
{	
	lpLastObj->wNumFaces=GetUINT16(file);

	lpLastObj->lpFaces=new DSFACE[lpLastObj->wNumFaces];
	lpLastObj->lpMat=new DS_MAT*[lpLastObj->wNumFaces];
	fread(lpLastObj->lpFaces,8,lpLastObj->wNumFaces,file);

	nSize-=(2+(8*lpLastObj->wNumFaces));
	printf("FacesChunk %i\n",lpLastObj->wNumFaces);

	int nDCnt=0;
	while(nSize)
	{
		if(MAX_RECD<=nDCnt)
		{
			printf("**** Reached MaxD in Faces Chunk\n");
			break;
		}

		DSUINT16 nL_ID=GetUINT16(file);
		DSUINT32 nL_Size=GetUINT32(file);

		switch(nL_ID)
		{
		case 0x4130:
			{
				DS_FACEMAT*	lpNew=new DS_FACEMAT();
				memset(lpNew,0,sizeof(DS_FACEMAT));
				if(lpLastObj->lpFirstFMat==0)
				{
					lpLastObj->lpFirstFMat=lpNew;
					lpLastObj->lpLastFMat=lpNew;
				}
				else
				{
					lpLastObj->lpLastFMat->lpNext=lpNew;
					lpNew->lpPrev=lpLastObj->lpLastFMat;
					lpLastObj->lpLastFMat=lpNew;
				}
				lpLastObj->nNumFMat++;

				char szBuffer[256];
				GetString(file,szBuffer,256);

				DS_MAT*	lpCurMat=lpFirstMat;
				DS_MAT*	lpUseMat=0;

				DSUINT32	i;

				for(i=0;i<dwNumMat;i++)
				{
					if(strcmp(lpCurMat->szName,szBuffer)==0)
					{
						lpUseMat=lpCurMat;
						break;
					}
					lpCurMat=lpCurMat->lpNext;
				}
				
				if(lpUseMat==0)
					printf("Can't find MapChunk (%s)\n",szBuffer);

				DSUINT16 wEntries=GetUINT16(file);
				lpNew->nFaces=wEntries;
				lpNew->lpMat=lpUseMat;
				lpNew->lpFace=new DSUINT16[wEntries];

				DSUINT16 wIdx;
				for(i=0;i<wEntries;i++)
				{
					wIdx=GetUINT16(file);
					lpNew->lpFace[i]=wIdx;
					lpLastObj->lpMat[wIdx]=lpUseMat;
				}
				printf("FacesMaterialChunk (%s) %i\n",szBuffer,wEntries);
				nSize-=nL_Size;
			}
			break;
		case 0x4150:
			printf("---- Unused Face Smoothing Chunk\n");
			SkipChunk(file,nL_ID,nL_Size-6);
			nSize-=nL_Size;
			break;
		default:
			printf("**** Unknown Faces Chunk :)\n");
			SkipChunk(file,nL_ID,nL_Size-6);
			nSize-=nL_Size;
			break;
		}
		nDCnt++;
	}
}

void C3DS_OBJECT::ReadTriMeshChunk(DSUINT16 nID,DSUINT32 nSize)
{
	int nDCnt=0;
	while(nSize)
	{
		if(MAX_RECD<=nDCnt)
		{
			printf("**** Reached MaxD in TriMesh Chunk\n");
			break;
		}

		DSUINT16 nL_ID=GetUINT16(file);
		DSUINT32 nL_Size=GetUINT32(file);

		switch(nL_ID)
		{
		case 0x4110:
			{
				lpLastObj->wNumVert=GetUINT16(file);
				lpLastObj->lpVert=new C3DS_VECTOR4[lpLastObj->wNumVert];
				for(int i=0;i<lpLastObj->wNumVert;i++)
				{
					C3DS_VECTOR4 vTmp;
					fread(&vTmp,12,1,file);

					float fTmp=vTmp.fVal[1];
					vTmp.fVal[1]=vTmp.fVal[2];
					vTmp.fVal[2]=fTmp;

					lpLastObj->lpVert[i]=vTmp;
				}

				printf("TriMesh with %i Vertices\n",lpLastObj->wNumVert);
				nSize-=nL_Size;
			}
			break;
		case 0x4140:
			{
				DSUINT16 nVertices=GetUINT16(file);
				lpLastObj->lpUV=new DSUV[nVertices];

				fread(lpLastObj->lpUV,8,nVertices,file);
				
				printf("TriMesh with %i Mapping Coords\n",nVertices);
				nSize-=nL_Size;
			}
			break;
		case 0x4160:
			{
				fread(&lpLastObj->vX,12,1,file);
				fread(&lpLastObj->vY,12,1,file);
				fread(&lpLastObj->vZ,12,1,file);
				fread(&lpLastObj->vO,12,1,file);

				printf("Object Coordinates X-Axis : %f,%f,%f\n",
					lpLastObj->vX.fVal[0],lpLastObj->vX.fVal[1],lpLastObj->vX.fVal[2]);
				printf("Object Coordinates Y-Axis : %f,%f,%f\n",
					lpLastObj->vY.fVal[0],lpLastObj->vY.fVal[1],lpLastObj->vY.fVal[2]);
				printf("Object Coordinates Z-Axis : %f,%f,%f\n",
					lpLastObj->vZ.fVal[0],lpLastObj->vZ.fVal[1],lpLastObj->vZ.fVal[2]);
				printf("Object Coordinates Origin : %f,%f,%f\n",
					lpLastObj->vO.fVal[0],lpLastObj->vO.fVal[1],lpLastObj->vO.fVal[2]);
				nSize-=nL_Size;
			}
			break;
		case 0x4120:
			{
				ReadFacesChunk(nL_ID,nL_Size-6);
				nSize-=nL_Size;
			}
			break;
		default:
			printf("**** Unknown TriMesh Chunk :)\n");
			SkipChunk(file,nL_ID,nL_Size-6);
			nSize-=nL_Size;
			break;
		}
		nDCnt++;
	}
}

void C3DS_OBJECT::ReadObjectChunk(DSUINT16 nID,DSUINT32 nSize)
{
	DS_OBJECT*	lpNewObj=new DS_OBJECT();
	memset(lpNewObj,0,sizeof(DS_OBJECT));
	if(lpLastObj==0)
	{
		lpLastObj=lpNewObj;
		lpFirstObj=lpNewObj;
		lpNewObj->lpNext=0;
		lpNewObj->lpPrev=0;
	}
	else
	{
		lpLastObj->lpNext=lpNewObj;
		lpNewObj->lpPrev=lpLastObj;
		lpNewObj->lpNext=0;
		lpLastObj=lpNewObj;
	}
	dwNumObj++;

	char szBuffer[256];
	GetString(file,szBuffer,256);
	printf("ObjectChunk: (%s)\n",szBuffer);
	lpLastObj->szName=strdup(szBuffer);
	nSize-=(strlen(szBuffer)+1);

	int nDCnt=0;
	while(nSize)
	{
		if(MAX_RECD<=nDCnt)
		{
			printf("**** Reached MaxD in Object Chunk\n");
			break;
		}

		DSUINT16 nL_ID=GetUINT16(file);
		DSUINT32 nL_Size=GetUINT32(file);

		switch(nL_ID)
		{
		case 0x4100:
			ReadTriMeshChunk(nL_ID,nL_Size-6);
			nSize-=nL_Size;
			break;
		case 0x4600:
			printf("---- Ignoring Light Chunk !\n");
			SkipChunk(file,nL_ID,nL_Size-6);
			nSize-=nL_Size;
			break;
		default:
			printf("**** Unknown Object Chunk :)\n");
			SkipChunk(file,nL_ID,nL_Size-6);
			nSize-=nL_Size;
			break;
		}
		nDCnt++;
	}
}

void C3DS_OBJECT::Read3DEditorChunk(DSUINT16 nID,DSUINT32 nSize)
{
	int nDCnt=0;
	while(nSize)
	{
		if(MAX_RECD<=nDCnt)
		{
			printf("**** Reached MaxD in 3DEditor Chunk\n");
			break;
		}

		DSUINT16 nL_ID=GetUINT16(file);
		DSUINT32 nL_Size=GetUINT32(file);

		switch(nL_ID)
		{
		case 0xAFFF:
			ReadMaterialChunk(nL_ID,nL_Size-6);
			nSize-=nL_Size;
			break;
		case 0x3D3E:
			nMeshVersion=GetUINT32(file);
			printf("Mesh Version : %i\n",nMeshVersion);
			nSize-=nL_Size;
			break;
		case 0x0100:
			{
				DSFLOAT32 fOneUnit=GetFLOAT32(file);
				printf("OneUnit : %f\n",fOneUnit);
				nSize-=nL_Size;
			}
			break;
		case 0x4000:
			ReadObjectChunk(nL_ID,nL_Size-6);
			nSize-=nL_Size;
			break;
		default:
			printf("**** Unknown 3DEditor Chunk :)\n");
			SkipChunk(file,nL_ID,nL_Size-6);
			nSize-=nL_Size;
			break;
		}
		nDCnt++;
	}
}

void C3DS_OBJECT::ReadMain3DSChunk(DSUINT16 nID,DSUINT32 nSize)
{
	int nDCnt=0;
	while(nSize)
	{
		if(MAX_RECD<=nDCnt)
		{
			printf("**** Reached MaxD in Main3DS Chunk\n");
			break;
		}


		DSUINT16 nL_ID=GetUINT16(file);
		DSUINT32 nL_Size=GetUINT32(file);

		switch(nL_ID)
		{
		case 0x0002:
			n3DSVersion=GetUINT32(file);
			printf("3DSVersion : %i\n",n3DSVersion);
			nSize-=nL_Size;
			break;
		case 0x3D3D:
			Read3DEditorChunk(nL_ID,nL_Size-6);
			nSize-=nL_Size;
			break;
		case 0xB000:
			printf("---- Ignoring Keyframer Chunk\n");
			SkipChunk(file,nL_ID,nL_Size-6);
			nSize-=nL_Size;
			break;
		default:
			printf("**** Unknown Main3DS Chunk :)\n");
			SkipChunk(file,nL_ID,nL_Size-6);
			nSize-=nL_Size;
			break;
		}
		nDCnt++;
	}
}

C3DS_OBJECT::C3DS_OBJECT(char* szFile)
{ 
	lpFirstMat=0;
	lpLastMat=0;
	dwNumMat=0;

	lpFirstObj=0;
	lpLastObj=0;
	dwNumObj=0;

	file=fopen(szFile,"rb");

	DSUINT16 nID=GetUINT16(file);
	DSUINT32 nSize=GetUINT32(file);

	if(nID==0x4D4D)
	{
		printf("3DS File Main Chunk 0x4D4D found with size %i\n",nSize);
		ReadMain3DSChunk(nID,nSize-6);
	}
	else
	{
		printf("**** %s is not an 3ds File !!!\n",szFile);
	}
	fclose(file);
}

C3DS_OBJECT::~C3DS_OBJECT()
{

	DS_MAT*		lpCurMat=lpFirstMat;
	DS_MAT*		lpNxtMat=0;
	DSUINT32 i;
	for(i=0;i<dwNumMat;i++)
	{
		lpNxtMat=lpCurMat->lpNext;
		free(lpCurMat->szName);
		free(lpCurMat->szFNam);

		delete lpCurMat;
		lpCurMat=lpNxtMat;
	}

	DS_OBJECT*		lpCurObj=lpFirstObj;
	DS_OBJECT*		lpNxtObj=0;
	for(i=0;i<dwNumObj;i++)
	{
		lpNxtObj=lpCurObj->lpNext;
		
		free(lpCurObj->szName);
		delete[] lpCurObj->lpVert;
		if(lpCurObj->lpUV) delete[] lpCurObj->lpUV;
		delete[] lpCurObj->lpFaces;

		DS_FACEMAT* lpCurFMat=lpCurObj->lpFirstFMat;
		DS_FACEMAT*	lpNxtFMat=0;
		for(int j=0;j<lpCurObj->nNumFMat;j++)
		{
			lpNxtFMat=lpCurFMat->lpNext;
			
			delete lpCurFMat->lpFace;
			delete lpCurFMat;

			lpCurFMat=lpNxtFMat;
		}

		delete lpCurObj;
		lpCurObj=lpNxtObj;
	}
}
