#include <math.h>
#include <stdlib.h>
#include "ZLC_RayTraceKernel.h"
#include "ZLC_Shader.h"

CZLC_SHADER::CZLC_SHADER() {};
CZLC_SHADER::~CZLC_SHADER() {};

CZLC_COLOR CZLC_SHD_MONOCHROM::GetColor(CZLA_RAY* lpRay,
										ZLA_FLOAT fT,CZLA_OBJECT* lpObj,CZLC_DATASET* lpDS,int nDepth)
{
	if(lpObj)
		return CZLC_COLOR(0.9f,0.9f,0.9f);
	else
		return CZLC_COLOR();
}

CZLC_COLOR CZLC_SHD_FLAT_UNLIT::GetColor(CZLA_RAY* lpRay,
										ZLA_FLOAT fT,CZLA_OBJECT* lpObj,CZLC_DATASET* lpDS,int nDepth)
{
	if(lpObj) return lpObj->GetColor(EZLAM_DIFFUSE,lpRay->GetPoint(fT));
	else return CZLC_COLOR();
}

CZLA_OBJECT* CZLC_SHD_PHONG::GetRayObjectIntersection(CZLC_DATASET* lpDS,CZLA_SRAY* lpSRay)
{
	ZLA_FLOAT fRP=0;
	CZLA_OBJECT* lpCurObj=(CZLA_OBJECT*)lpDS->llObjects.lpFirstNode;

	for(unsigned int i=0;i<lpDS->llObjects.nNumNodes;i++)
	{
		if(lpCurObj->GetRayIntersection(&fRP,*lpSRay,lpSRay->cInt))
		{
			if(lpCurObj->IsColorAvaible(EZLAM_TRANSPARENT))
			{
				CZLA_VECTOR4 vSP=lpSRay->GetPoint(fRP);
				CZLC_COLOR cSpec=lpCurObj->GetColor(EZLAM_TRANSPARENT,vSP);
				if(cSpec.NotZero())
				{
					lpCurObj=(CZLA_OBJECT*)lpCurObj->lpNextNode;
					continue;
				}
			}

			return lpCurObj;
		}
		lpCurObj=(CZLA_OBJECT*)lpCurObj->lpNextNode;
	}
	return 0;
}

CZLC_COLOR CZLC_SHD_PHONG::ShadowRay(CZLA_VECTOR4 vSP,CZLA_OBJECT* lpObj,CZLC_DATASET* lpDS)
{
	CZLC_COLOR cCol;
	
	CZLA_LIGHT* lpCurLight=(CZLA_LIGHT*)lpDS->llLight.lpFirstNode;

	CZLA_VECTOR4 vNormal=lpObj->GetUnitVectorNormal(vSP);

	for(unsigned int l=0;l<lpDS->llLight.nNumNodes;l++)
	{
		CZLA_SRAY cRay=lpCurLight->GetShadowRay(vSP);

		if(GetRayObjectIntersection(lpDS,&cRay)==0)
		{
			CZLC_COLOR cDiff=lpObj->GetColor(EZLAM_DIFFUSE,vSP);
			//vNormal*=((cDiff.fR+cDiff.fG+cDiff.fB)/3.0f)+0.5f;

			ZLA_FLOAT fDot=vNormal*cRay.vD;
			CZLC_COLOR cMul=cDiff*lpCurLight->cEmissive;
			CZLC_COLOR cAdd=cMul*fDot;
			cCol+=cAdd;
		}
		
		lpCurLight=(CZLA_LIGHT*)lpCurLight->lpNextNode;
	}
	return cCol;
}

CZLC_COLOR CZLC_SHD_PHONG::GetColor(CZLA_RAY* lpRay,
										ZLA_FLOAT fT,CZLA_OBJECT* lpObj,CZLC_DATASET* lpDS,int nDepth)
{
	CZLC_COLOR cCol;
	CZLA_VECTOR4 vSP=lpRay->GetPoint(fT);

	if(lpObj)
	{
		CZLA_VECTOR4 vNormal=lpObj->GetUnitVectorNormal(vSP);

		cCol=CZLC_KERNEL::cAmbiCol;
		if(lpObj->IsColorAvaible(EZLAM_TRANSPARENT))
		{
			CZLA_VECTOR4 vD=lpRay->vD.Reflect(vNormal);
			CZLA_RAY cReflRay(vSP,vD);

			ZLA_FLOAT fDN=lpRay->vD*vNormal;
			
			if(fDN<0)
			{
				CZLA_RAY cRefrRay(vSP,lpRay->vD.Refract(vNormal,CZLC_KERNEL::fFractionalIndex,lpObj->fN));
				CZLC_COLOR cTran=CZLC_KERNEL::GetRayColor(&cRefrRay,lpDS,nDepth+1);
				CZLC_COLOR cR=lpObj->GetColor(EZLAM_TRANSPARENT,vSP);
				CZLC_COLOR cSpec=CZLC_KERNEL::GetRayColor(&cReflRay,lpDS,nDepth+1);

				CZLC_COLOR cMul1=cR*cTran;
				CZLC_COLOR cMul2=(1-cR)*cSpec;
				CZLC_COLOR cAdd=cMul1+cMul2;
				cCol+=cAdd;
			}
			else
			{
				CZLA_RAY cRefrRay(vSP,lpRay->vD.Refract(-vNormal,lpObj->fN,CZLC_KERNEL::fFractionalIndex));
				CZLC_COLOR cTran=CZLC_KERNEL::GetRayColor(&cRefrRay,lpDS,nDepth+1);
				CZLC_COLOR cR=lpObj->GetColor(EZLAM_TRANSPARENT,vSP);
				CZLC_COLOR cSpec=CZLC_KERNEL::GetRayColor(&cReflRay,lpDS,nDepth+1);

				CZLC_COLOR cMul1=cR*cTran;
				CZLC_COLOR cMul2=(1.0-cR)*cSpec;
				CZLC_COLOR cAdd=cMul1+cMul2;
				cCol+=cAdd;
			}
		}
		if(lpObj->IsColorAvaible(EZLAM_DIFFUSE))
		{
			CZLC_COLOR cAdd=ShadowRay(vSP,lpObj,lpDS);
			cCol+=cAdd;	
		}
		if(lpObj->IsColorAvaible(EZLAM_SPECULAR))
		{
			CZLA_VECTOR4 vD=lpRay->vD.Reflect(vNormal);
			CZLA_RAY cReflRay(vSP,vD);
			CZLC_COLOR cSpec=CZLC_KERNEL::GetRayColor(&cReflRay,lpDS,nDepth+1);
			CZLC_COLOR cAdd=lpObj->GetColor(EZLAM_SPECULAR,vSP);
			cSpec*=cAdd;

			/*ZLA_FLOAT fDN=lpRay->vD*-vNormal;
			CZLC_COLOR cRefl=cSpec+(1.0-cSpec)*pow(1.0-fDN,5); // schlicks aprox for stainless alloy
			cCol+=cRefl;*/
			cCol+=cSpec;
			
		}
	}
	return cCol;
}
