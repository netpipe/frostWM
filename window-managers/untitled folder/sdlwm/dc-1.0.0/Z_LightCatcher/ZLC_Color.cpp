#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "ZLC_Color.h"
#include "ZLA_Types.h"
#include "DCLib.h"

extern CDCL_INIFILE cDefaultIni;

#define ZLC_COL_CLAMP(x) {if(x<0) x=0;if(x>=1) x=1.0-FLT_EPSILON;}

CZLC_COLOR::CZLC_COLOR()
{
	fR=0;
	fG=0;
	fB=0;
} 

CZLC_COLOR::CZLC_COLOR(ZLA_FLOAT fInitR,ZLA_FLOAT fInitG,ZLA_FLOAT fInitB)
{
	fR=fInitR;
	fG=fInitG;
	fB=fInitB;
}

CZLC_COLOR::~CZLC_COLOR() {}

long CZLC_COLOR::GetRGB()
{
	unsigned char cR,cG,cB;
	ZLA_FLOAT fCR=fR;
	ZLA_FLOAT fCG=fG;
	ZLA_FLOAT fCB=fB;

	ZLC_COL_CLAMP(fCR);
	ZLC_COL_CLAMP(fCG);
	ZLC_COL_CLAMP(fCB);
	

	/*cR=(unsigned char)(256.0*fCR);
	cG=(unsigned char)(256.0*fCG);
	cB=(unsigned char)(256.0*fCB);*/

	// Gamma corrected ^_^
	HDCL_INISEC hSec=cDefaultIni.GetSection("General");
	char* szGamma=cDefaultIni.GetStringValue(hSec,"Gamma");
	
	ZLA_FLOAT fGamma=atof(szGamma);
	cR=int(256.0*pow(fCR,1.0/fGamma));
	cG=int(256.0*pow(fCG,1.0/fGamma));
	cB=int(256.0*pow(fCB,1.0/fGamma));

	return (cR<<16)|(cG<<8)|(cB);
}

CZLC_COLOR CZLC_COLOR::operator+=(CZLC_COLOR& cOp)
{
	fR+=cOp.fR;
	fG+=cOp.fG;
	fB+=cOp.fB;
	return *this;
}

CZLC_COLOR CZLC_COLOR::operator+(CZLC_COLOR& cOp)
{
	return CZLC_COLOR(fR+cOp.fR,fG+cOp.fG,fB+cOp.fB);
}

CZLC_COLOR CZLC_COLOR::operator*=(CZLC_COLOR& cOp)
{
	fR*=cOp.fR;
	fG*=cOp.fG;
	fB*=cOp.fB;
	return *this;
}

CZLC_COLOR CZLC_COLOR::operator*(ZLA_FLOAT fOp)
{
	return CZLC_COLOR(fR*fOp,fG*fOp,fB*fOp);
}

CZLC_COLOR CZLC_COLOR::operator*(CZLC_COLOR& cOp)
{
	return CZLC_COLOR(fR*cOp.fR,fG*cOp.fG,fB*cOp.fB);
}

CZLC_COLOR operator-(ZLA_FLOAT fOp,CZLC_COLOR cOp)
{
	return CZLC_COLOR(fOp-cOp.fR,fOp-cOp.fG,fOp-cOp.fB);
}

bool CZLC_COLOR::NotZero()
{
	return (fR!=0.0||fG!=0.0||fB!=0.0);
}