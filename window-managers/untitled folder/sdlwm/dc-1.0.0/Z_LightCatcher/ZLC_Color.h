#ifndef _ZLC_COLOR_H
#define _ZLC_COLOR_H

#include "ZLA_Types.h"

class CZLC_COLOR;

CZLC_COLOR operator-(ZLA_FLOAT fOp,CZLC_COLOR cOp);

class CZLC_COLOR
{
public:
	CZLC_COLOR();
	CZLC_COLOR(ZLA_FLOAT fInitR,ZLA_FLOAT fInitG,ZLA_FLOAT fInitB);
	~CZLC_COLOR();

	long GetRGB();
	bool NotZero();
	CZLC_COLOR operator+=(CZLC_COLOR& cOp);
	CZLC_COLOR operator*(ZLA_FLOAT fOp);
	CZLC_COLOR operator*(CZLC_COLOR& cOp);
	CZLC_COLOR operator*=(CZLC_COLOR& cOp);
	CZLC_COLOR operator+(CZLC_COLOR& cOp);

	friend CZLC_COLOR operator-(ZLA_FLOAT fOp,CZLC_COLOR cOp);

	ZLA_FLOAT fR;
	ZLA_FLOAT fG;
	ZLA_FLOAT fB;
};

#endif