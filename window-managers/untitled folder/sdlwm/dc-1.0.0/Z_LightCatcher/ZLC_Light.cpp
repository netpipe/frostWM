#include "ZLC_Light.h"

CZLA_LT_POINT::CZLA_LT_POINT(CZLA_VECTOR4 vPos,CZLC_COLOR cCol)
{
	vOrigin=vPos;
	cEmissive=cCol;
}

CZLA_SRAY CZLA_LT_POINT::GetShadowRay(CZLA_VECTOR4 vSP)
{
	CZLA_VECTOR4 vD=vOrigin-vSP;
	ZLA_FLOAT fL=!vD;
	vD=vD/fL;

	return CZLA_SRAY(vSP,vD,ZLAFL_EPSILON2,fL);
}

CZLA_LT_PARALLEL::CZLA_LT_PARALLEL(CZLA_VECTOR4 vDir,CZLC_COLOR cCol)
{
	vDirection=vDir;
	cEmissive=cCol;
}

CZLA_SRAY CZLA_LT_PARALLEL::GetShadowRay(CZLA_VECTOR4 vSP)
{
	return CZLA_SRAY(vSP,vDirection*-1,ZLAFL_EPSILON2,ZLAFL_MAX);
}