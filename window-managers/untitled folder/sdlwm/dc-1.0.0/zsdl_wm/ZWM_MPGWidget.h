#ifdef HAVE_LIBSMPEG

#ifndef _ZWM_MPGWIDGET_H
#define _ZWM_MPGWIDGET_H

#include "ZWM_Window.h"

#include "smpeg.h"

class CZWM_MPGWIDGET:public CZWM_WINDOW
{
public:
	CZWM_MPGWIDGET(int nIndex,CZWM_WINDOW* lpPar,SDL_Rect rcRect);
	~CZWM_MPGWIDGET();
	void Update();
	void LoadMPG(char *szFile,bool bAutoSize);

	SMPEG*						lpMPG;
	SMPEG_Info					mpgInfo;

	int							nIdx;
};

#endif _ZWM_MPGWIDGET_H
#endif // _ZWM_SMPEG