#ifndef _ZWM_BUTTON_COLLECTION_H
#define _ZWM_BUTTON_COLLECTION_H

#include "ZWM_Window.h"

class CZWM_BUTTON_COLLECTION:public CZWM_WINDOW
{
public:
	CZWM_BUTTON_COLLECTION(char* szBmp,SDL_Rect rcButton,int nSizeX,int nSizeY,
		ZWM_MSGCALLBACK lpCalllback,int nX,int nY);

	SDL_Rect			rcBtn;
	int					nSX,nSY;
	int					nCurSel;
};

#endif
