#include "ZLA_Ray.h"

CZLA_RAY::CZLA_RAY():vO(),vD()
{
	
}

CZLA_RAY::CZLA_RAY(CZLA_VECTOR4 vOrigin,CZLA_VECTOR4 vDirection):vO(vOrigin),vD(vDirection)
{
	
}

CZLA_VECTOR4 CZLA_RAY::GetPoint(ZLA_FLOAT fT)
{
	CZLA_VECTOR4 vR=fT*vD;
	return vO+vR;
}

CZLA_SRAY::CZLA_SRAY(CZLA_VECTOR4 vOrigin,CZLA_VECTOR4 vDirection,ZLA_FLOAT fStart,ZLA_FLOAT fEnd):
	cInt(fStart,fEnd),CZLA_RAY(vOrigin,vDirection)
{

}
