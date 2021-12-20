#include <math.h>
#include "ZLA_Matrix4x4.h"
#include "ZLA_Types.h"

CZLA_MATRIX4x4::CZLA_MATRIX4x4()
{
	fMat[0][0]=fMat[1][1]=fMat[2][2]=fMat[3][3]=1.0f;
	fMat[0][1]=fMat[0][2]=fMat[0][3]=fMat[1][0]=0.0f;
	fMat[1][2]=fMat[1][3]=fMat[2][0]=fMat[2][1]=0.0f;
	fMat[2][3]=fMat[3][0]=fMat[3][1]=fMat[3][2]=0.0f;
}

CZLA_MATRIX4x4 CZLA_MATRIX4x4::operator *(CZLA_MATRIX4x4& mOp)
{
	CZLA_MATRIX4x4 mRet;
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			mRet.fMat[i][j]=((fMat[i][0]*mOp.fMat[0][j])+
				(fMat[i][1]*mOp.fMat[1][j])+
				(fMat[i][2]*mOp.fMat[2][j])+
				(fMat[i][3]*mOp.fMat[3][j]));
		}
	}
	return mRet;
}

void CZLA_MATRIX4x4::SetZero()
{
	fMat[0][0]=fMat[0][1]=fMat[0][2]=fMat[0][3]=0.0f;
	fMat[1][0]=fMat[1][1]=fMat[1][2]=fMat[1][3]=0.0f;
	fMat[2][0]=fMat[2][1]=fMat[2][2]=fMat[2][3]=0.0f;
	fMat[3][0]=fMat[3][1]=fMat[3][2]=fMat[3][3]=0.0f; 
}

void CZLA_MATRIX4x4::SetIdentity()
{
	fMat[0][0]=fMat[1][1]=fMat[2][2]=fMat[3][3]=1.0f;
	fMat[0][1]=fMat[0][2]=fMat[0][3]=fMat[1][0]=0.0f;
	fMat[1][2]=fMat[1][3]=fMat[2][0]=fMat[2][1]=0.0f;
	fMat[2][3]=fMat[3][0]=fMat[3][1]=fMat[3][2]=0.0f;
}

void CZLA_MATRIX4x4::SetScale(ZLA_FLOAT fSX,ZLA_FLOAT fSY,ZLA_FLOAT fSZ)
{
	fMat[0][0]=fSX;
	fMat[1][1]=fSY;
	fMat[2][2]=fSZ;
	fMat[3][3]=1.0f;

	fMat[0][1]=fMat[0][2]=fMat[0][3]=fMat[1][0]=0.0f;
	fMat[1][2]=fMat[1][3]=fMat[2][0]=fMat[2][1]=0.0f;
	fMat[2][3]=fMat[3][0]=fMat[3][1]=fMat[3][2]=0.0f;
}

void CZLA_MATRIX4x4::SetTranslation(ZLA_FLOAT fTX,ZLA_FLOAT fTY,ZLA_FLOAT fTZ)
{
	fMat[3][0]=fTX;
	fMat[3][1]=fTY;
	fMat[3][2]=fTZ;
	
	fMat[0][0]=fMat[1][1]=fMat[2][2]=fMat[3][3]=1.0f;
	fMat[0][1]=fMat[0][2]=fMat[0][3]=fMat[1][0]=0.0f;
	fMat[1][2]=fMat[1][3]=fMat[2][0]=fMat[2][1]=0.0f;
	fMat[2][3]=0.0f;
}

void CZLA_MATRIX4x4::SetXRotation(ZLA_FLOAT fAlpha)
{
#ifdef ZLA_DOUBLE_MODE
	ZLA_FLOAT fS=sin(fAlpha);
	ZLA_FLOAT fC=cos(fAlpha);
#else
	ZLA_FLOAT fS=sinf(fAlpha);
	ZLA_FLOAT fC=cosf(fAlpha);
#endif
	
	fMat[1][1]=fMat[2][2]=fC;
	fMat[1][2]=fS;
	fMat[2][1]=-fS;
	fMat[0][0]=fMat[3][3]=1.0f;

	fMat[0][1]=fMat[0][2]=fMat[0][3]=fMat[1][0]=0.0f;
	fMat[1][3]=fMat[2][0]=fMat[2][3]=fMat[3][0]=0.0f;
	fMat[3][1]=fMat[3][2]=0.0f;
}

void CZLA_MATRIX4x4::SetYRotation(ZLA_FLOAT fAlpha)
{
#ifdef ZLA_DOUBLE_MODE
	ZLA_FLOAT fS=sin(fAlpha);
	ZLA_FLOAT fC=cos(fAlpha);
#else
	ZLA_FLOAT fS=sinf(fAlpha);
	ZLA_FLOAT fC=cosf(fAlpha);
#endif

	fMat[0][0]=fMat[2][2]=fC;
	fMat[2][0]=fS;
	fMat[0][2]=-fS;
	fMat[1][1]=fMat[3][3]=1.0f;

	fMat[0][1]=fMat[0][3]=fMat[1][0]=fMat[1][2]=0.0f;
	fMat[1][3]=fMat[2][1]=fMat[2][3]=fMat[3][0]=0.0f;
	fMat[3][1]=fMat[3][2]=0.0f;
}

void CZLA_MATRIX4x4::SetZRotation(ZLA_FLOAT fAlpha)
{
#ifdef ZLA_DOUBLE_MODE
	ZLA_FLOAT fS=sin(fAlpha);
	ZLA_FLOAT fC=cos(fAlpha);
#else
	ZLA_FLOAT fS=sinf(fAlpha);
	ZLA_FLOAT fC=cosf(fAlpha);
#endif

	fMat[0][0]=fMat[1][1]=fC;
	fMat[0][1]=fS;
	fMat[1][0]=-fS;
	fMat[2][2]=fMat[3][3]=1.0f;

	fMat[0][2]=fMat[0][3]=fMat[1][2]=fMat[1][3]=0.0f;
	fMat[2][0]=fMat[2][1]=fMat[2][3]=fMat[3][0]=0.0f;
	fMat[3][1]=fMat[3][2]=0.0f;
}