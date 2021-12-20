#ifndef _ZLA_INTERVAL_H
#define _ZLA_INTERVAL_H

#include "ZLA_Types.h"

class CZLA_INTERVAL
{
public:
	ZLA_FLOAT fS,fE;

	CZLA_INTERVAL();
	CZLA_INTERVAL(ZLA_FLOAT fStart,ZLA_FLOAT fEnd);
	bool IsIn(ZLA_FLOAT fT);
	ZLA_FLOAT GetClosest(ZLA_FLOAT fT1,ZLA_FLOAT fT2);
};

#endif