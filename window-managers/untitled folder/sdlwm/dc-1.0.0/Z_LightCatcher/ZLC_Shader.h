#ifndef _ZLC_SHADER_H
#define _ZLC_SHADER_H

#include "ZLC_Color.h"
#include "ZLA_Module.h"

// virtual shader class
class CZLC_SHADER
{
public:
	CZLC_SHADER();
	~CZLC_SHADER();

	virtual CZLC_COLOR GetColor(CZLA_RAY* lpRay,ZLA_FLOAT fTT,CZLA_OBJECT* lpObj,CZLC_DATASET* lpDS,int nDepth)=0;
};

// monochrom shader
class CZLC_SHD_MONOCHROM:public CZLC_SHADER
{
public:
	CZLC_COLOR GetColor(CZLA_RAY* lpRay,ZLA_FLOAT fT,CZLA_OBJECT* lpObj,CZLC_DATASET* lpDS,int nDepth);
};

// flat unlit shader
class CZLC_SHD_FLAT_UNLIT:public CZLC_SHADER
{
public:
	CZLC_COLOR GetColor(CZLA_RAY* lpRay,ZLA_FLOAT fT,CZLA_OBJECT* lpObj,CZLC_DATASET* lpDS,int nDepth);
};

// phong shader
class CZLC_SHD_PHONG:public CZLC_SHADER
{
public:
	CZLC_COLOR GetColor(CZLA_RAY* lpRay,ZLA_FLOAT fT,CZLA_OBJECT* lpObj,CZLC_DATASET* lpDS,int nDepth);
	CZLC_COLOR ShadowRay(CZLA_VECTOR4 vSP,CZLA_OBJECT* lpObj,CZLC_DATASET* lpDS);
	CZLA_OBJECT* GetRayObjectIntersection(CZLC_DATASET* lpDS,CZLA_SRAY* lpSRay);
};

#endif