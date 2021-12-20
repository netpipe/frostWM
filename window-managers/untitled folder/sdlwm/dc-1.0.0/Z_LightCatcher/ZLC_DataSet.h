#ifndef _ZLC_DATASET_H
#define _ZLC_DATASET_H

#include "ZSDL_WM.h"
#include "Load3DS.h"

class CZLC_DATASET
{
public:
	CZLC_DATASET();
	~CZLC_DATASET();

	void Merge3DSFile(char* szFile);

	CZWM_LLIST	llObjects;
	CZWM_LLIST	llLight;
};

#endif