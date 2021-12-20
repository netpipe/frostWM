#ifndef _ZLC_VIEWPORT_H
#define _ZLC_VIEWPORT_H

#include "ZSDL_WM.h"
#include "ZLC_DataSet.h"

class CZLC_VIEWPORT
{
public:
	CZLC_VIEWPORT(int nWidth,int nHeight);
	~CZLC_VIEWPORT();
	static int ViewportWndCallback(CZWM_WINDOW* lpWnd,int nMsg,int nParam1,int nParam2);
	void RayTrace(CZLC_DATASET* lpDS);

	CZWM_WINDOW*	lpWnd;
	int				nW,nH;
};

#endif