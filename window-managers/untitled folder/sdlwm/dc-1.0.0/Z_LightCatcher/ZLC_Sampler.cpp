#include "ZLC_RayTraceKernel.h"
#include "ZLC_Sampler.h"

CZLC_SAMPLER::CZLC_SAMPLER() {}
CZLC_SAMPLER::~CZLC_SAMPLER() {}

CZLC_COLOR CZLC_SMP_SINGLERAY::TracePxl(int nX,int nY,CZLC_DATASET* lpDS)
{
	CZLC_COLOR cCol;

	CZLA_RAY cRay=CZLC_KERNEL::lpCurCam->GetPixelRay((float)nX,(float)nY);
	cCol=CZLC_KERNEL::GetRayColor(&cRay,lpDS,0);

	return cCol;
}

CZLC_COLOR CZLC_SMP_POLYRAY::TracePxl(int nX,int nY,CZLC_DATASET* lpDS)
{
	CZLC_COLOR cCol[4];

	
	CZLA_RAY cRay1=CZLC_KERNEL::lpCurCam->GetPixelRay((float)(nX-0.25),(float)(nY-0.25));
	cCol[0]=CZLC_KERNEL::GetRayColor(&cRay1,lpDS,0);
	CZLA_RAY cRay2=CZLC_KERNEL::lpCurCam->GetPixelRay((float)(nX+0.25),(float)(nY+0.25));
	cCol[1]=CZLC_KERNEL::GetRayColor(&cRay2,lpDS,0);
	CZLA_RAY cRay3=CZLC_KERNEL::lpCurCam->GetPixelRay((float)(nX-0.25),(float)(nY+0.25));
	cCol[2]=CZLC_KERNEL::GetRayColor(&cRay3,lpDS,0);
	CZLA_RAY cRay4=CZLC_KERNEL::lpCurCam->GetPixelRay((float)(nX+0.25),(float)(nY-0.25));
	cCol[3]=CZLC_KERNEL::GetRayColor(&cRay4,lpDS,0);

	return (cCol[0]+cCol[1]+cCol[2]+cCol[3])*0.25;
}