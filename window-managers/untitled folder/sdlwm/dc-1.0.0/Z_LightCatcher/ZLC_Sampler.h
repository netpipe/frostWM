#ifndef _ZLC_SAMPLER_H
#define _ZLC_SAMPLER_H

#include "ZLC_Color.h"
#include "ZLC_DataSet.h"

class CZLC_SAMPLER
{
public:
	CZLC_SAMPLER();
	~CZLC_SAMPLER();

	virtual CZLC_COLOR TracePxl(int nX,int nY,CZLC_DATASET* lpDS)=0;
};

class CZLC_SMP_SINGLERAY:public CZLC_SAMPLER
{
public:
	CZLC_COLOR TracePxl(int nX,int nY,CZLC_DATASET* lpDS);
};

class CZLC_SMP_POLYRAY:public CZLC_SAMPLER
{
public:
	CZLC_COLOR TracePxl(int nX,int nY,CZLC_DATASET* lpDS);
};

#endif