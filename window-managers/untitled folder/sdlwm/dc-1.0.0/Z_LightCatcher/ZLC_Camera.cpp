#include "ZLC_RayTraceKernel.h"
#include "ZLC_Camera.h"

// virtual Object Camera
CZLC_CAMERA::CZLC_CAMERA() 
{
	vCamOrigin.fVec[0]=0;
	vCamOrigin.fVec[1]=0;
	vCamOrigin.fVec[2]=0;
	vCamOrigin.fVec[3]=1;
}
CZLC_CAMERA::~CZLC_CAMERA() {}

// Orthogonal Camera
CZLC_CAM_ORTHOGONAL::CZLC_CAM_ORTHOGONAL() {}
CZLC_CAM_ORTHOGONAL::~CZLC_CAM_ORTHOGONAL() {}

CZLA_RAY CZLC_CAM_ORTHOGONAL::GetPixelRay(float fX,float fY)
{
	CZLA_RAY cRay;
	cRay.vO=vCamOrigin;

	ZLA_FLOAT fSX=1;
	ZLA_FLOAT fSY=1;

	cRay.vO.fVec[0]=(fX-(CZLC_KERNEL::lpCurViewPort->nW/(ZLA_FLOAT)2.0))*fSX;
	cRay.vO.fVec[1]=(-(fY-(CZLC_KERNEL::lpCurViewPort->nH/(ZLA_FLOAT)2.0)))*fSY;
	cRay.vO.fVec[2]=0;
	cRay.vD=CZLA_VECTOR4(0,0,1,1);
	return cRay;
}

// Pinhole Camera
CZLC_CAM_PINHOLE::CZLC_CAM_PINHOLE() {}
CZLC_CAM_PINHOLE::~CZLC_CAM_PINHOLE() {}

CZLA_RAY CZLC_CAM_PINHOLE::GetPixelRay(float fX,float fY)
{
	CZLA_RAY cRay;
	cRay.vO=vCamOrigin;

	ZLA_FLOAT au=-128, av=-128;
	ZLA_FLOAT bu= 128, bv= 128;

	ZLA_FLOAT nx=CZLC_KERNEL::lpCurViewPort->nW;
	ZLA_FLOAT ny=CZLC_KERNEL::lpCurViewPort->nH;
	ZLA_FLOAT s=40;

	ZLA_FLOAT fU=au+(bu-au)*(fX/(nx-1));
	ZLA_FLOAT fV=av+(bv-av)*(fY/(ny-1));
	cRay.vD=CZLA_VECTOR4(fU,fV,s,1);
	cRay.vD/=!cRay.vD;
	return cRay;
}