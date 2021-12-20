#ifndef _ZLA_MATRIX4X4_H
#define _ZLA_MATRIX4X4_H

#include "ZLA_Vector4.h"
#include "ZLA_Types.h"

class CZLA_VECTOR4;
class CZLA_MATRIX4x4;

class CZLA_MATRIX4x4
{
public:
	CZLA_MATRIX4x4();
	ZLA_FLOAT fMat[4][4];
	CZLA_MATRIX4x4 operator *(CZLA_MATRIX4x4& mOp);
	CZLA_VECTOR4 operator *(CZLA_VECTOR4& vOp);
	
	void SetZero();
	void SetIdentity();
	void SetScale(ZLA_FLOAT fSX,ZLA_FLOAT fSY,ZLA_FLOAT fSZ);
	void SetTranslation(ZLA_FLOAT fTX,ZLA_FLOAT fTY,ZLA_FLOAT fTZ);
	void SetXRotation(ZLA_FLOAT fAlpha);
	void SetYRotation(ZLA_FLOAT fAlpha);
	void SetZRotation(ZLA_FLOAT fAlpha);
};

#endif
