#ifndef _ZLA_PLANE_H
#define _ZLA_PLANE_H

#include "ZLA_Object.h"
#include "ZLA_Vector4.h"

class CZLA_PLANE:public CZLA_OBJECT
{
public:
	CZLA_VECTOR4 vA,vN;

	CZLA_PLANE();
	CZLA_PLANE(CZLA_VECTOR4 vAnchor,CZLA_VECTOR4 vNormal);
	CZLA_PLANE(CZLA_VECTOR4 vAnchor,CZLA_VECTOR4 vU,CZLA_VECTOR4 vV);

	bool GetRayIntersection(ZLA_FLOAT* fRP,CZLA_RAY& rRay,CZLA_INTERVAL& iInv);
	CZLA_VECTOR4 GetUnitVectorNormal(CZLA_VECTOR4& vP);
};

#endif