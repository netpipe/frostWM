#include "ZLA_Plane.h"

CZLA_PLANE::CZLA_PLANE():vN()
{

}

CZLA_PLANE::CZLA_PLANE(CZLA_VECTOR4 vAnchor,CZLA_VECTOR4 vU,CZLA_VECTOR4 vV):
	vN(vU%vV),vA(vAnchor)
{

}

CZLA_PLANE::CZLA_PLANE(CZLA_VECTOR4 vAnchor,CZLA_VECTOR4 vNormal):
	vN(vNormal),vA(vAnchor)
{

}

bool CZLA_PLANE::GetRayIntersection(ZLA_FLOAT* fRP,CZLA_RAY& rRay,CZLA_INTERVAL& iInv)
{
	ZLA_FLOAT fDen=rRay.vD*vN;
	if((fDen<ZLAFL_EPSILON)&&(fDen>-ZLAFL_EPSILON))
		return false;	// The Ray is parallel to the plain; no intersection occures
						// The Ray possibly is in the plane ! (nominator==0)
	else
	{
		ZLA_FLOAT fT=((vA-rRay.vO)*vN)/fDen;
		if(iInv.IsIn(fT))
		{
			*fRP=fT;
			return true; // Ray hits the surface
		}
		else return false; // Ray hits the surface, but is not in the interval
	}
}

CZLA_VECTOR4 CZLA_PLANE::GetUnitVectorNormal(CZLA_VECTOR4& vP)
{
	return vN;
}