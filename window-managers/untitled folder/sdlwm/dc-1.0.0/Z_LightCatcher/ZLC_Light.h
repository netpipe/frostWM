#ifndef _ZLC_LIGHT_H
#define _ZLC_LIGHT_H

#include "ZLA_Ray.h"
#include "ZLA_Interval.h"
#include "ZSDL_WM.h"
#include "ZLC_Color.h"

class CZLA_LIGHT:public CZWM_LL_NODE
{
public:
	virtual CZLA_SRAY GetShadowRay(CZLA_VECTOR4 vSP)=0;

	CZLC_COLOR		cEmissive; // Emitted Light
};

class CZLA_LT_POINT:public CZLA_LIGHT
{
public:
	CZLA_LT_POINT(CZLA_VECTOR4 vPos,CZLC_COLOR cCol);
	CZLA_SRAY GetShadowRay(CZLA_VECTOR4 vSP);

	CZLA_VECTOR4 vOrigin;
};

class CZLA_LT_PARALLEL:public CZLA_LIGHT
{
public:
	CZLA_LT_PARALLEL(CZLA_VECTOR4 vDir,CZLC_COLOR cCol);
	CZLA_SRAY GetShadowRay(CZLA_VECTOR4 vSP);

	CZLA_VECTOR4 vDirection;
};

#endif