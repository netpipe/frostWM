#include "ZLA_Triangle.h"

CZLA_TRIANGLE::CZLA_TRIANGLE():vP1(),vP2(),vP3()
{

}

CZLA_TRIANGLE::CZLA_TRIANGLE(CZLA_VECTOR4 vPoint1,CZLA_VECTOR4 vPoint2,CZLA_VECTOR4 vPoint3):
	vP1(vPoint1),vP2(vPoint2),vP3(vPoint3)
{
	
}

bool CZLA_TRIANGLE::GetRayIntersection(ZLA_FLOAT* fRP,CZLA_RAY& rRay,CZLA_INTERVAL& iInv)
{
	CZLA_VECTOR4 vE1=vP2-vP1;
	CZLA_VECTOR4 vE2=vP3-vP1;

	CZLA_VECTOR4 vP=rRay.vD%vE2;

	ZLA_FLOAT fDet=vE1*vP;

	CZLA_VECTOR4 vT=rRay.vO-vP1;
	ZLA_FLOAT fIDet=(ZLA_FLOAT)1.0/fDet;
   
	CZLA_VECTOR4 vQ=vT%vE1;

	ZLA_FLOAT fU,fV;
      
	if(fDet>ZLAFL_EPSILON)
	{
		fU=vT*vP;
		if(fU<0.0||fU>fDet) return false; //  plane hit, but outside the triangle border
            
		fV=rRay.vD*vQ;
		if(fV<0.0||fU+fV>fDet) return false; // plane hit, but outside the triangle border
	}
	else if(fDet<-ZLAFL_EPSILON)
	{
		fU=vT*vP;
		if(fU>0.0||fU<fDet) return false; //  plane hit, but outside the triangle border
      
		fV=rRay.vD*vQ;
		if(fV>0.0||fU+fV<fDet) return false; // plane hit, but outside the triangle border
	}
	else return false;  // Ray is parallel to the triangle

	ZLA_FLOAT fT=(vE2*vQ)*fIDet;
	fU*=fIDet;
	fV*=fIDet;

	if(iInv.IsIn(fT))
	{
		*fRP=fT;
		return true; // Ray hit the triangle and the parameter is within the interval
	}
	else return false; // Ray hit the triangle, but outside the interval
}

CZLA_VECTOR4 CZLA_TRIANGLE::GetUnitVectorNormal(CZLA_VECTOR4& vP)
{
	CZLA_VECTOR4 vA=vP3-vP1;
	CZLA_VECTOR4 vB=vP2-vP1;
	CZLA_VECTOR4 vN=(vA)%(vB);
	return vN/!vN;
}
