#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "ZLC_Material.h"

CZLA_MATERIAL::CZLA_MATERIAL():nType(EZLAMT_NONE),cCol(0,0,0) {}

CZLA_MATERIAL::CZLA_MATERIAL(CZLC_COLOR nNewCol):nType(EZLAMT_COLOR),cCol(nNewCol) {}

CZLC_COLOR CZLA_MATERIAL::GetColor(CZLA_VECTOR4 vP)
{
	switch(nType)
	{
	case EZLAMT_NONE: return CZLC_COLOR();
	default: return cCol;
	}
}

CZLA_MAT_STRIPE::CZLA_MAT_STRIPE(CZLC_COLOR c1,CZLC_COLOR c2,int nNewMSType,float fW):
	nMS_Type(nNewMSType),fMS_W(fW),cMS_Col1(c1),cMS_Col2(c2)
{
	
}

CZLC_COLOR CZLA_MAT_STRIPE::GetColor(CZLA_VECTOR4 vP)
{
	
	if(nMS_Type==0)
	{
		if(sin(vP.fVec[0])>0) return cMS_Col1; else return cMS_Col2;
	}
	else if(nMS_Type==1)
	{
		if(sin(3.141592654*vP.fVec[0]/fMS_W)>0) return cMS_Col1; else return cMS_Col2;
	}
	else if(nMS_Type==2)
	{
		ZLA_FLOAT fT=(1+sin(3.141592654*vP.fVec[0]/fMS_W))/2;
		CZLC_COLOR cA=cMS_Col1*fT;
		CZLC_COLOR cB=cMS_Col2*(1.0-fT);
		return cA+cB;
//		return (cMS_Col1*fT)+(cMS_Col2*(1-fT));
	}
	return CZLC_COLOR();
}

CZLA_MAT_SOLIDNOISE::CZLA_MAT_SOLIDNOISE(CZLC_COLOR c1,CZLC_COLOR c2,ZLA_FLOAT fS):
	cMSN_Col1(c1),cMSN_Col2(c2),fMSN_S(fS)
{
	srand(time(NULL));
	int i;

	for(i=0;i<256;i++)
	{
		nPermInt[i]=i;

		ZLA_FLOAT fLen=0;
		do
		{
			vRandUV[i]=CZLA_VECTOR4
			(
				2.0*(((ZLA_FLOAT)rand())/((ZLA_FLOAT)RAND_MAX))-1.0,
				2.0*(((ZLA_FLOAT)rand())/((ZLA_FLOAT)RAND_MAX))-1.0,
				2.0*(((ZLA_FLOAT)rand())/((ZLA_FLOAT)RAND_MAX))-1.0,
				1
			);
			fLen=!vRandUV[i];
			if(fLen<1.0)
			{
				vRandUV[i]/=fLen;
				break;
			}
		}while(fLen>1.0);
	}

	for(i=0;i<256;i++)
	{
		int nR1=rand()%256;
		int nR2=rand()%256;
		int nT=nPermInt[nR1];
		nPermInt[nR1]=nPermInt[nR2];
		nPermInt[nR2]=nT;
	}
}

ZLA_FLOAT CZLA_MAT_SOLIDNOISE::Omega(ZLA_FLOAT fT)
{
	ZLA_FLOAT fAT=fabs(fT);
	if(fAT<1)
	{
		return 2.0*(fAT*fAT*fAT)-3.0*(fAT*fAT)+1.0;
	}
	else return 0;
}

int CZLA_MAT_SOLIDNOISE::Phi(int nI)
{
	int nRet=nPermInt[abs(nI)%256];
	return nRet;
}

ZLA_FLOAT CZLA_MAT_SOLIDNOISE::Ohm(ZLA_FLOAT fU,ZLA_FLOAT fV,ZLA_FLOAT fW,
								   ZLA_FLOAT fI,ZLA_FLOAT fJ,ZLA_FLOAT fK)
{
	int nIdx=Phi((int)fI+Phi((int)fJ+Phi((int)fK)));
	//nIdx=0;
	ZLA_FLOAT fF=Omega(fU)*Omega(fV)*Omega(fW);
	CZLA_VECTOR4 vA=CZLA_VECTOR4(fU,fV,fW,1);
	ZLA_FLOAT fRet=vRandUV[nIdx]*vA;
	return fF*fRet; 
}

CZLC_COLOR CZLA_MAT_SOLIDNOISE::GetColor(CZLA_VECTOR4 vP)
{
	vP*=fMSN_S;
	ZLA_FLOAT fN=0;
	for(int i=(int)fabs(vP.fVec[0]);i<(int)fabs(vP.fVec[0])+1;i++)
	{
		for(int j=(int)fabs(vP.fVec[1]);j<(int)fabs(vP.fVec[1])+1;j++)
		{
			for(int k=(int)fabs(vP.fVec[2]);k<(int)fabs(vP.fVec[2])+1;k++)
			{
				fN+=Ohm(fabs(vP.fVec[0])-i,fabs(vP.fVec[1])-j,fabs(vP.fVec[2])-k,i,j,k);
			}
		}
	}
	ZLA_FLOAT fAN=fabs(fN);
	CZLC_COLOR cA=cMSN_Col1*fAN;
	CZLC_COLOR cB=cMSN_Col2*(1.0-fAN);
	return cA+cB;
}
