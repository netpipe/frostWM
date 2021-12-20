#include <math.h>
#include "ZLA_Interval.h"

CZLA_INTERVAL::CZLA_INTERVAL():fS(0),fE(0)
{

}

CZLA_INTERVAL::CZLA_INTERVAL(ZLA_FLOAT fStart,ZLA_FLOAT fEnd):fS(fStart),fE(fEnd)
{

}

bool CZLA_INTERVAL::IsIn(ZLA_FLOAT fT)
{
	if(fS<fE)
	{
		if(fT>fS&&fT<fE) return true; else return false;
	}
	else
	{
		if(fT>fE&&fT<fS) return true; else return false;
	}
}

ZLA_FLOAT CZLA_INTERVAL::GetClosest(ZLA_FLOAT fT1,ZLA_FLOAT fT2)
{
#ifdef ZLA_DOUBLE_MODE
	ZLA_FLOAT fD1=fabs(fS-fT1);
	ZLA_FLOAT fD2=fabs(fS-fT2);
#else
	ZLA_FLOAT fD1=fabsf(fS-fT1);
	ZLA_FLOAT fD2=fabsf(fS-fT2);
#endif
	if(fD1<fD2) return fT1; else return fT2;
}