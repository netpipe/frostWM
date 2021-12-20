#ifndef _ZLA_SPHERE_H
#define _ZLA_SPHERE_H

#include "ZLA_Object.h"
#include "ZLA_Vector4.h"

class CZLA_SPHERE:public CZLA_OBJECT
{
public:
	CZLA_VECTOR4 vC;
	ZLA_FLOAT fR;

	CZLA_SPHERE();
	CZLA_SPHERE(CZLA_VECTOR4 vOrigin,ZLA_FLOAT fRadius);

	bool GetRayIntersection(ZLA_FLOAT* fRP,CZLA_RAY& rRay,CZLA_INTERVAL& iInv);
	CZLA_VECTOR4 GetUnitVectorNormal(CZLA_VECTOR4& vP);
};

#endif