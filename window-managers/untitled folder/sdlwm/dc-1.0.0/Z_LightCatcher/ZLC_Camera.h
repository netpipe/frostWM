#ifndef _ZLC_CAMERA_H
#define _ZLC_CAMERA_H

#include "ZLA_Module.h"

class CZLC_CAMERA
{
public:
	CZLC_CAMERA();
	~CZLC_CAMERA();

	virtual CZLA_RAY GetPixelRay(float fX,float fY)=0;

	CZLA_VECTOR4	vCamOrigin;
};

class CZLC_CAM_ORTHOGONAL:public CZLC_CAMERA
{
public:
	CZLC_CAM_ORTHOGONAL();
	~CZLC_CAM_ORTHOGONAL();
	CZLA_RAY GetPixelRay(float fX,float fY);
};

class CZLC_CAM_PINHOLE:public CZLC_CAMERA
{
public:
	CZLC_CAM_PINHOLE();
	~CZLC_CAM_PINHOLE();
	CZLA_RAY GetPixelRay(float fX,float fY);
};

#endif