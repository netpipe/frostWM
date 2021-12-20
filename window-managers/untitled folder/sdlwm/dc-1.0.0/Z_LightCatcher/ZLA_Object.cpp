#include "ZLA_Object.h"

CZLA_OBJECT::CZLA_OBJECT():fN(1.0),lpDiffMat(0),lpSpecMat(0),lpTranMat(0)
{
	
}

CZLC_COLOR CZLA_OBJECT::GetColor(int nMat,CZLA_VECTOR4 vP)
{
	switch(nMat)
	{
	case EZLAM_NONE:
		return CZLC_COLOR();
	case EZLAM_DIFFUSE:
		if(lpDiffMat) return lpDiffMat->GetColor(vP); else return CZLC_COLOR();
	case EZLAM_SPECULAR:
		if(lpSpecMat) return lpSpecMat->GetColor(vP); else return CZLC_COLOR();
	case EZLAM_TRANSPARENT:
		if(lpTranMat) return lpTranMat->GetColor(vP); else return CZLC_COLOR();
	}
	return CZLC_COLOR();
}

bool CZLA_OBJECT::IsColorAvaible(int nMat)
{
	switch(nMat)
	{
	case EZLAM_NONE:
		return false;
	case EZLAM_DIFFUSE:
		return (lpDiffMat!=0);
	case EZLAM_SPECULAR:
		return (lpSpecMat!=0);
	case EZLAM_TRANSPARENT:
		return (lpTranMat!=0);
	default:
		return false;
	}
}

void CZLA_OBJECT::SetMaterial(int nMat,CZLA_MATERIAL* lpMat)
{
	switch(nMat)
	{
	case EZLAM_NONE:
		return;
	case EZLAM_DIFFUSE:
		lpDiffMat=lpMat; return;
	case EZLAM_SPECULAR:
		lpSpecMat=lpMat; return;
	case EZLAM_TRANSPARENT:
		lpTranMat=lpMat; return;
	}
}