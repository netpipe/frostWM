#ifndef _ZLA_OBJECT_H
#define _ZLA_OBJECT_H

#include "ZLA_Ray.h"
#include "ZLA_Interval.h"
#include "ZSDL_WM.h"
#include "ZLC_Material.h"

enum EZLA_MAT
{
	EZLAM_NONE,
	EZLAM_DIFFUSE,
	EZLAM_SPECULAR,
	EZLAM_TRANSPARENT
};

class CZLA_OBJECT:public CZWM_LL_NODE
{
public:
	CZLA_OBJECT();
	virtual bool GetRayIntersection(ZLA_FLOAT* fRP,CZLA_RAY& rRay,CZLA_INTERVAL& iInv)=0;
	virtual CZLA_VECTOR4 GetUnitVectorNormal(CZLA_VECTOR4& vP)=0;
	CZLC_COLOR GetColor(int nMat,CZLA_VECTOR4 vP);
	bool IsColorAvaible(int nMat);
	void SetMaterial(int nMat,CZLA_MATERIAL* lpMat);

	ZLA_FLOAT		fN;			// Fraction Index
private:
	CZLA_MATERIAL*	lpDiffMat;	// Diffuse -> direct lighting
	CZLA_MATERIAL*	lpSpecMat;	// Specular -> mirrored light
	CZLA_MATERIAL*	lpTranMat;	// Transparent -> refracted light
};

#endif