#ifndef _ZWM_OBJECT_H
#define _ZWM_OBJECT_H

#include <SDL.h>
#include "ZWM_Types.h"

class CZWM_WINDOW;

class CZWM_WIDGET:public CZWM_LL_NODE
{
public:
	CZWM_WIDGET();
	~CZWM_WIDGET();

	void GetClientRect(SDL_Rect* rcC);

	SDL_Rect		rcWnd;
	SDL_Surface*	lpSurface;
	void*			lpPrivateData;

	CZWM_LLIST		llChildList;
	CZWM_WINDOW*	lpParent;
	int				nWndType;
	SDL_Rect		rcTmp;

	const char*		szTitle;
};

#endif