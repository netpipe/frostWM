#include <math.h>
#include "ZLA_Vector4.h"


CZLA_VECTOR4::CZLA_VECTOR4()
{
	fVec[0]=fVec[1]=fVec[2]=0.0f;
	fVec[3]=1.0f;
}

CZLA_VECTOR4::CZLA_VECTOR4(ZLA_FLOAT fX,ZLA_FLOAT fY,ZLA_FLOAT fZ,ZLA_FLOAT fW=1.0f)
{
	fVec[0]=fX;
	fVec[1]=fY;
	fVec[2]=fZ;
	fVec[3]=fW;
}

CZLA_VECTOR4 CZLA_VECTOR4::operator +(CZLA_VECTOR4& vOp)
{
	return CZLA_VECTOR4(
		fVec[0]+vOp.fVec[0],
		fVec[1]+vOp.fVec[1],
		fVec[2]+vOp.fVec[2],1);
}

CZLA_VECTOR4 CZLA_VECTOR4::operator -(CZLA_VECTOR4& vOp)
{
	return CZLA_VECTOR4(
		fVec[0]-vOp.fVec[0],
		fVec[1]-vOp.fVec[1],
		fVec[2]-vOp.fVec[2],1);
}

ZLA_FLOAT CZLA_VECTOR4::operator *(CZLA_VECTOR4& vOp)
{
	return ((fVec[0]*vOp.fVec[0])+(fVec[1]*vOp.fVec[1])+(fVec[2]*vOp.fVec[2]));
}

CZLA_VECTOR4 CZLA_VECTOR4::operator %(CZLA_VECTOR4& vOp)
{
	return CZLA_VECTOR4(
		fVec[1]*vOp.fVec[2]-fVec[2]*vOp.fVec[1],
		fVec[2]*vOp.fVec[0]-fVec[0]*vOp.fVec[2],
		fVec[0]*vOp.fVec[1]-fVec[1]*vOp.fVec[0],1);
}

CZLA_VECTOR4 CZLA_VECTOR4::operator *(ZLA_FLOAT fOp)
{
	return CZLA_VECTOR4(fVec[0]*fOp,fVec[1]*fOp,fVec[2]*fOp,1);
}

CZLA_VECTOR4 CZLA_VECTOR4::operator /(ZLA_FLOAT fOp)
{
	return CZLA_VECTOR4(fVec[0]/fOp,fVec[1]/fOp,fVec[2]/fOp,1);
}

CZLA_VECTOR4 operator *(ZLA_FLOAT fOp,CZLA_VECTOR4& vOp)
{
	return CZLA_VECTOR4(vOp.fVec[0]*fOp,vOp.fVec[1]*fOp,vOp.fVec[2]*fOp,1);
}

CZLA_VECTOR4 CZLA_VECTOR4::operator +=(CZLA_VECTOR4& vOp)
{
	fVec[0]+=vOp.fVec[0];
	fVec[1]+=vOp.fVec[1];
	fVec[2]+=vOp.fVec[2];
	return *this;
}

CZLA_VECTOR4 CZLA_VECTOR4::operator -=(CZLA_VECTOR4& vOp)
{
	fVec[0]-=vOp.fVec[0];
	fVec[1]-=vOp.fVec[1];
	fVec[2]-=vOp.fVec[2];
	return *this;
}

CZLA_VECTOR4 CZLA_VECTOR4::operator *=(ZLA_FLOAT fOp)
{
	fVec[0]*=fOp;
	fVec[1]*=fOp;
	fVec[2]*=fOp;
	return *this;
}

CZLA_VECTOR4 CZLA_VECTOR4::operator /=(ZLA_FLOAT fOp)
{
	fVec[0]/=fOp;
	fVec[1]/=fOp;
	fVec[2]/=fOp;
	return *this;
}

CZLA_VECTOR4 CZLA_VECTOR4::operator *(CZLA_MATRIX4x4& mOp)
{
	return CZLA_VECTOR4
	(
		(fVec[0]*mOp.fMat[0][0])+(fVec[1]*mOp.fMat[1][0])+(fVec[2]*mOp.fMat[2][0]),
		(fVec[0]*mOp.fMat[0][1])+(fVec[1]*mOp.fMat[1][1])+(fVec[2]*mOp.fMat[2][1]),
		(fVec[0]*mOp.fMat[0][2])+(fVec[1]*mOp.fMat[1][2])+(fVec[2]*mOp.fMat[2][2]),1
	);
}

ZLA_FLOAT CZLA_VECTOR4::operator !()
{
#ifdef ZLA_DOUBLE_MODE
	return sqrt((fVec[0]*fVec[0])+(fVec[1]*fVec[1])+(fVec[2]*fVec[2]));
#else
	return sqrtf((fVec[0]*fVec[0])+(fVec[1]*fVec[1])+(fVec[2]*fVec[2]));
#endif
}

CZLA_VECTOR4 CZLA_VECTOR4::operator -()
{
	return CZLA_VECTOR4
	(
		-fVec[0],-fVec[1],-fVec[2],fVec[3]
	);
}

CZLA_VECTOR4 CZLA_VECTOR4::Reflect(CZLA_VECTOR4 vPNormal)
{
	ZLA_FLOAT fPNL=!vPNormal;
	ZLA_FLOAT fDN=(*this)*vPNormal;
	CZLA_VECTOR4 vA=(((fDN)/(fPNL*fPNL))*vPNormal)*2;
	return (*this)-vA;
}

CZLA_VECTOR4 CZLA_VECTOR4::Refract(CZLA_VECTOR4 vPNormal,ZLA_FLOAT fN1,ZLA_FLOAT fN2)
{
	CZLA_VECTOR4& vD=*this;
	ZLA_FLOAT fDN=vD*vPNormal;

#ifdef ZLA_DOUBLE_MODE
	ZLA_FLOAT fSQRT=sqrt(1.0-(((fN1*fN1)*(1-(fDN*fDN)))/(fN2*fN2)));
#else
	ZLA_FLOAT fSQRT=sqrtf(1.0f-(((fN1*fN1)*(1-(fDN*fDN)))/(fN2*fN2)));
#endif
	CZLA_VECTOR4 vA=vPNormal*fSQRT;
	CZLA_VECTOR4 vB=(vPNormal*fDN)/fN2;
	CZLA_VECTOR4 vC=vD-vB;
	CZLA_VECTOR4 vE=fN1*vC;
	return (vE/fN2)-vA;
}
