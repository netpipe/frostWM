#include <math.h>
#include "ZLA_Sphere.h"
#include "ZLA_Types.h"

CZLA_SPHERE::CZLA_SPHERE():vC(),fR(0)
{

}

CZLA_SPHERE::CZLA_SPHERE(CZLA_VECTOR4 vCenter,ZLA_FLOAT fRadius):vC(vCenter),fR(fRadius)
{

}

bool CZLA_SPHERE::GetRayIntersection(ZLA_FLOAT* fRP,CZLA_RAY& rRay,CZLA_INTERVAL& iInv)
{
	CZLA_VECTOR4& vD=rRay.vD;
	CZLA_VECTOR4& vO=rRay.vO;

	CZLA_VECTOR4 vCO=vC-vO;
	ZLA_FLOAT fDD=vD*vD;
	ZLA_FLOAT fDis=((vD*vCO)*(vD*vCO))-(fDD)*((vCO*vCO)-(fR*fR));

	if(fDis<0) return false; // Ray didn't hit the sphere
	else if((fDis<ZLAFL_EPSILON)&&(fDis>ZLAFL_EPSILON))
	{
		ZLA_FLOAT fT=(-vD*vCO)/(fDD);
		if(iInv.IsIn(fT))
		{
			*fRP=fT;
			return true; // One Ray hit, which occured in the interval
		}
		else return false; // One Ray hit, but outside the interval
	}
	else
	{
		CZLA_VECTOR4 vR=vO-vC;
		ZLA_FLOAT fLP=-vD*(vR);
#ifdef ZLA_DOUBLE_MODE
		ZLA_FLOAT fSQRTDis=sqrt(fDis);
#else
		ZLA_FLOAT fSQRTDis=sqrtf(fDis);
#endif

		ZLA_FLOAT fT1=(fLP-fSQRTDis)/fDD;
		ZLA_FLOAT fT2=(fLP+fSQRTDis)/fDD;
		bool bInt1=iInv.IsIn(fT1);
		bool bInt2=iInv.IsIn(fT2);
		if(bInt1&&bInt2)
		{
			*fRP=iInv.GetClosest(fT1,fT2);
			return true; // Ray hit twice, the closest interval hit is taken
		}
		else if(bInt1)
		{
			*fRP=fT1;
			return true; // Ray hit twice, but only the first hit is in the interval
		}
		else if(bInt2)
		{
			*fRP=fT2;
			return true; // Ray hit twice, but only the second hit is in the interval
		}
		else return false; // Ray hit twice, but none of the hits were inside the interval
	}
}

CZLA_VECTOR4 CZLA_SPHERE::GetUnitVectorNormal(CZLA_VECTOR4& vP)
{
	return (vP-vC)/fR;
}
