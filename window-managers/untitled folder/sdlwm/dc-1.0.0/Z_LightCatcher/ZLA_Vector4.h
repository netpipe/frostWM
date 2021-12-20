#ifndef _ZLA_VECTOR4_H
#define _ZLA_VECTOR4_H

#include "ZLA_Matrix4x4.h"
#include "ZLA_Types.h"

class CZLA_VECTOR4;
class CZLA_MATRIX4x4;

CZLA_VECTOR4 operator*(ZLA_FLOAT fOp,CZLA_VECTOR4& vOp);

class CZLA_VECTOR4
{
public:
	ZLA_FLOAT fVec[4];
	CZLA_VECTOR4();
	CZLA_VECTOR4(ZLA_FLOAT fX,ZLA_FLOAT fY,ZLA_FLOAT fZ,ZLA_FLOAT fW);

	CZLA_VECTOR4 operator +(CZLA_VECTOR4& vOp);
	CZLA_VECTOR4 operator -(CZLA_VECTOR4& vOp);
	ZLA_FLOAT operator *(CZLA_VECTOR4& vOp);
	CZLA_VECTOR4 operator %(CZLA_VECTOR4& vOp);
	CZLA_VECTOR4 operator *(ZLA_FLOAT fOp);
	CZLA_VECTOR4 operator /(ZLA_FLOAT fOp);
	friend CZLA_VECTOR4 operator *(ZLA_FLOAT fOp,CZLA_VECTOR4& vOp);
	CZLA_VECTOR4 operator +=(CZLA_VECTOR4& vOp);
	CZLA_VECTOR4 operator -=(CZLA_VECTOR4& vOp);
	CZLA_VECTOR4 operator *=(ZLA_FLOAT fOp);
	CZLA_VECTOR4 operator /=(ZLA_FLOAT fOp);
	CZLA_VECTOR4 operator -();
	ZLA_FLOAT operator !();
	CZLA_VECTOR4 operator *(CZLA_MATRIX4x4& mOp);
	CZLA_VECTOR4 Reflect(CZLA_VECTOR4 vPNormal);
	CZLA_VECTOR4 Refract(CZLA_VECTOR4 vPNormal,ZLA_FLOAT fN1,ZLA_FLOAT fN2);
};

#endif
