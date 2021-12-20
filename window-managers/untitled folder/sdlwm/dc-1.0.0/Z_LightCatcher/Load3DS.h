#ifndef _LOAD3DS_H
#define _LOAD3DS_H

typedef unsigned long	DSUINT32;
typedef unsigned short	DSUINT16;
typedef unsigned char	DSUINT8;
typedef float			DSFLOAT32;

class CFINAL_LIGHTMAP;
class C76LIGHTMAP;

struct UI8RGB
{
	unsigned char r,g,b;
};

struct DSUV
{
	float U,V;
};

struct C3DS_VECTOR4
{
	float	fVal[4];
};

struct DS_MAT
{
	char*		szName;
	char*		szFNam;
	UI8RGB		cAmb;
	UI8RGB		cDiff;
	UI8RGB		cSpec;
	
	DSUINT16	pShiny;
	DSUINT16	pShinyStr;
	DSUINT16	pTrans;
	DSUINT16	pTransFall;
	DSUINT16	pRefBlur;
	DSUINT16	wRenderType;
	DSUINT16	pSelfIlu;
	DSFLOAT32	fWireThickness;

	DS_MAT*		lpNext;
	DS_MAT*		lpPrev;
};

struct DSFACE
{
	DSUINT16	wA,wB,wC;
	DSUINT16	wFlags;
};

struct DS_FACEMAT
{
	DS_MAT*				lpMat;
	DSUINT16*			lpFace;
	DSUINT16			nFaces;

	CFINAL_LIGHTMAP*	lpFinalLM;
	C76LIGHTMAP*		lpTmpLM;
	unsigned int		dwLM;

	DS_FACEMAT*			lpNext;
	DS_FACEMAT*			lpPrev;
};

struct DS_OBJECT
{
	char*				szName;
	C3DS_VECTOR4*		lpVert;
	DSUV*				lpUV;
	DSUINT16			wNumVert;

	C3DS_VECTOR4		vX,vY,vZ,vO;

	DSFACE*				lpFaces;
	DS_MAT**			lpMat;
	DSUINT16			wNumFaces;

	DS_FACEMAT*			lpFirstFMat;
	DS_FACEMAT*			lpLastFMat;
	DSUINT16			nNumFMat;

	DS_OBJECT*			lpNext;
	DS_OBJECT*			lpPrev;
};

class C3DS_OBJECT
{
public:

	FILE*		file;
	DSUINT32	n3DSVersion;
	DSUINT32	nMeshVersion;
	
	DS_MAT*		lpFirstMat;
	DS_MAT*		lpLastMat;
	DSUINT32	dwNumMat;

	DS_OBJECT*	lpFirstObj;
	DS_OBJECT*	lpLastObj;
	DSUINT32	dwNumObj;

	C3DS_OBJECT(char* szName);
	~C3DS_OBJECT();

	void ReadMain3DSChunk(DSUINT16 nID,DSUINT32 nSize);
	void Read3DEditorChunk(DSUINT16 nID,DSUINT32 nSize);
	void ReadMaterialChunk(DSUINT16 nID,DSUINT32 nSize);
	void ReadColorContainerChunk(DSUINT16 nID,DSUINT32 nSize);
	void ReadPercentContainerChunk(DSUINT16 nID,DSUINT32 nSize);
	void ReadMapChunk(DSUINT16 nID,DSUINT32 nSize);
	void ReadObjectChunk(DSUINT16 nID,DSUINT32 nSize);
	void ReadTriMeshChunk(DSUINT16 nID,DSUINT32 nSize);
	void ReadFacesChunk(DSUINT16 nID,DSUINT32 nSize);
};

#endif