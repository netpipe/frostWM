#ifndef _ZLA_RAY_H
#define _ZLA_RAY_H

#include "ZLA_Vector4.h"
#include "ZLA_Interval.h"

class CZLA_RAY
{
public:
	CZLA_VECTOR4 vO,vD;

	CZLA_RAY();
	CZLA_RAY(CZLA_VECTOR4 vOrigin,CZLA_VECTOR4 vDirection);
	
	
	CZLA_VECTOR4 GetPoint(ZLA_FLOAT fT);
};

class CZLA_SRAY:public CZLA_RAY
{
public:
	CZLA_SRAY(CZLA_VECTOR4 vOrigin,CZLA_VECTOR4 vDirection,ZLA_FLOAT fStart,ZLA_FLOAT fEnd);
	CZLA_INTERVAL cInt;
};

#endif