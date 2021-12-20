#include <stdlib.h>
#include "ZLC_RayTraceKernel.h"
#include "DCLib.h"

extern CDCL_INIFILE cDefaultIni;

CZLC_CAMERA*	CZLC_KERNEL::lpCurCam=0;
CZLC_SAMPLER*	CZLC_KERNEL::lpCurSampler=0;
CZLC_VIEWPORT*	CZLC_KERNEL::lpCurViewPort=0;
CZLC_SHADER*	CZLC_KERNEL::lpCurShader=0;
SDL_Thread*		CZLC_KERNEL::lpThread=0;
CZLC_COLOR		CZLC_KERNEL::cAmbiCol;
ZLA_FLOAT		CZLC_KERNEL::fFractionalIndex=1.0; // vacuum

CZLC_KERNEL::CZLC_KERNEL()
{
	HDCL_INISEC hSec=cDefaultIni.GetSection("General");
	char* szAmbStr=cDefaultIni.GetStringValue(hSec,"AmbientLight");
#ifdef ZLA_DOUBLE_MODE
	sscanf(szAmbStr,"(%lf,%lf,%lf)",&cAmbiCol.fR,&cAmbiCol.fG,&cAmbiCol.fB);
#else
	sscanf(szAmbStr,"(%f,%f,%f)",&cAmbiCol.fR,&cAmbiCol.fG,&cAmbiCol.fB);
#endif
}

CZLC_KERNEL::~CZLC_KERNEL()
{
	if(lpCurCam) delete	lpCurCam;
	if(lpCurSampler) delete lpCurSampler;
	if(lpCurViewPort) delete lpCurViewPort;
	if(lpCurShader) delete lpCurShader;
}

void CZLC_KERNEL::SetCamera(CZLC_CAMERA* lpNewCam)
{
	if(lpCurCam) delete lpCurCam;
	lpCurCam=lpNewCam;
}

void CZLC_KERNEL::SetSampler(CZLC_SAMPLER* lpNewSampler)
{
	if(lpCurSampler) delete lpCurSampler;
	lpCurSampler=lpNewSampler;
}

void CZLC_KERNEL::SetViewPort(CZLC_VIEWPORT* lpViewPort)
{
	if(lpCurViewPort) delete lpCurViewPort;
	lpCurViewPort=lpViewPort;
}

void CZLC_KERNEL::SetShader(CZLC_SHADER* lpShader)
{
	if(lpCurShader) delete lpCurShader;
	lpCurShader=lpShader;
}

void CZLC_KERNEL::SetAmbientColor(CZLC_COLOR cNewCol)
{
	cAmbiCol=cNewCol;
}

CZLC_COLOR CZLC_KERNEL::GetRayColor(CZLA_RAY* lpRay,CZLC_DATASET* lpDS,int nDepth)
{
	CZLC_COLOR cCol((unsigned char)0,(unsigned char)0,(unsigned char)0);
	if(nDepth>5) return cCol;

	CZLA_OBJECT*	lpCurObj=(CZLA_OBJECT*)lpDS->llObjects.lpFirstNode;
	CZLA_OBJECT*	lpFirstHit=0;
	ZLA_FLOAT		fFirstHit=0;

	CZLA_INTERVAL cInt(ZLAFL_EPSILON2,ZLAFL_MAX);
	
	int nDbgObjID=0;
	for(unsigned int i=0;i<lpDS->llObjects.nNumNodes;i++)
	{
		ZLA_FLOAT fRP;

		if(lpCurObj->GetRayIntersection(&fRP,(*lpRay),cInt))
		{
			lpFirstHit=lpCurObj;
			fFirstHit=fRP;
			cInt.fE=fRP;
			nDbgObjID=i+1;
		}
		lpCurObj=(CZLA_OBJECT*)lpCurObj->lpNextNode;
	}

	cCol=lpCurShader->GetColor(lpRay,fFirstHit,lpFirstHit,lpDS,nDepth+1);

	return cCol;
}

int CZLC_KERNEL::ThreadMain(void* lpData)
{
	lpCurViewPort->RayTrace((CZLC_DATASET*)lpData);
	return 0;
}

void CZLC_KERNEL::StartTracing(CZLC_DATASET* lpDS)
{
	lpThread=SDL_CreateThread(ThreadMain,lpDS);
}