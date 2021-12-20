#ifndef _ZLA_TRIANGLE_H
#define _ZLA_TRIANGLE_H

#include "ZLA_Object.h"

class CZLA_TRIANGLE:public CZLA_OBJECT
{
public:
	CZLA_VECTOR4 vP1,vP2,vP3;
	CZLA_TRIANGLE();
	CZLA_TRIANGLE(CZLA_VECTOR4 vPoint1,CZLA_VECTOR4 vPoint2,CZLA_VECTOR4 vPoint3);

	bool GetRayIntersection(ZLA_FLOAT* fRP,CZLA_RAY& rRay,CZLA_INTERVAL& iInv);
	CZLA_VECTOR4 GetUnitVectorNormal(CZLA_VECTOR4& vP);
};

#endif